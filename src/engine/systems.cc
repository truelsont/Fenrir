#include "engine/systems.hh"
#include "engine/components.hh"
#include "util/logger/logger.hh"

#include <algorithm>
#include <cmath>
#include <string>

namespace fenrir {
namespace systems {

// ---------------------------------------------------------------------------
// Economy systems
// ---------------------------------------------------------------------------

void register_economy_systems(flecs::world& ecs) {
    // Daily economy update: recompute province economic outputs.
    // Tax income   = population * development * 0.001
    // Production   = development * terrain_modifier * 0.5
    //   (terrain_modifier baked into development at init)
    // Trade value  = production * local_trade_power * 0.1
    ecs.system<Economy, const Population, const ProvinceTag>("EconomyUpdate")
        .each([](Economy& econ, const Population& pop, const ProvinceTag&) {
            // Tax income: larger population & higher development yield more
            float tax_efficiency = 1.0f;  // Could be modified by tech later
            econ.tax_income = static_cast<float>(pop.total) *
                              econ.development * tax_efficiency * 0.001f;

            // Production output
            econ.production = econ.development * 0.5f;

            // Trade value derives from production and local trade power
            float trade_modifier = 1.0f + econ.local_trade_power * 0.1f;
            econ.trade_value = econ.production * trade_modifier;
        });
}

// ---------------------------------------------------------------------------
// Population systems
// ---------------------------------------------------------------------------

void register_population_systems(flecs::world& ecs) {
    // Monthly population growth.
    // The system runs every tick (daily) but only applies growth on
    // day boundaries that cross a month (every 30 ticks).
    //
    // Growth rate = base_rate * (1 - war_penalty) * health_modifier
    // base_rate defaults to 0.2% per month for pre-industrial societies.
    //
    // We use a "monthly pulse" phase component to avoid per-entity timers.
    // For simplicity, growth is applied every 30 ticks by the Simulation
    // class gating via Flecs pipeline intervals.

    ecs.system<Population, const Economy, const ProvinceOwner, const ProvinceTag>(
            "PopulationGrowth")
        .interval(30.0f)  // Every 30 days (roughly monthly)
        .each([](Population& pop, const Economy& econ,
                 const ProvinceOwner&, const ProvinceTag&) {
            if (pop.total == 0) return;

            // Health modifier scales with development (better cities = less
            // disease) clamped 0.8 - 1.2
            float health_modifier =
                std::clamp(0.8f + econ.development * 0.04f, 0.8f, 1.2f);

            // Monthly growth (base 0.2%)
            float monthly_growth = pop.growth_rate * health_modifier;

            int32_t delta =
                static_cast<int32_t>(static_cast<float>(pop.total) *
                                     monthly_growth);
            // Ensure at least +1 for small provinces
            if (delta == 0 && pop.total > 0) delta = 1;

            pop.total = static_cast<uint32_t>(
                std::max(0, static_cast<int32_t>(pop.total) + delta));
        });
}

// ---------------------------------------------------------------------------
// Military systems
// ---------------------------------------------------------------------------

void register_military_systems(flecs::world& ecs) {
    // Province manpower generation (monthly).
    // monthly_manpower = max_manpower * 0.01
    // Province manpower is capped at its max.
    ecs.system<Military, const ProvinceTag>("ManpowerRecovery")
        .interval(30.0f)
        .each([](Military& mil, const ProvinceTag&) {
            float recovery = mil.supply_limit * 0.01f;
            mil.manpower = std::min(mil.manpower + recovery,
                                    mil.supply_limit);
        });

    // Army attrition: troops exceeding supply limit take losses.
    // attrition = max(0, (troops - supply_limit) / troops * 0.05) per month
    ecs.system<ArmyStrength, const ArmyPosition, const ArmyTag>(
            "ArmyAttrition")
        .interval(30.0f)
        .each([&ecs](ArmyStrength& str, const ArmyPosition& pos,
                     const ArmyTag&) {
            // Find the province this army is in and check supply
            float supply_limit = 10000.0f;  // default

            // Look up province supply limit
            ecs.each([&](const ProvinceTag& ptag, const Military& mil) {
                if (ptag.id == pos.province_id) {
                    supply_limit = mil.supply_limit;
                }
            });

            float total_troops = static_cast<float>(
                str.infantry + str.cavalry + str.artillery);
            if (total_troops <= 0.0f) return;

            float excess = total_troops - supply_limit;
            if (excess > 0.0f) {
                float attrition_rate = (excess / total_troops) * 0.05f;

                str.infantry -= static_cast<uint32_t>(
                    static_cast<float>(str.infantry) * attrition_rate);
                str.cavalry -= static_cast<uint32_t>(
                    static_cast<float>(str.cavalry) * attrition_rate);
                str.artillery -= static_cast<uint32_t>(
                    static_cast<float>(str.artillery) * attrition_rate);
            }
        });

    // Morale recovery: +0.05 per month when not in combat.
    // (Combat detection not implemented yet; always recover for now.)
    ecs.system<ArmyStrength, const ArmyTag>("MoraleRecovery")
        .interval(30.0f)
        .each([](ArmyStrength& str, const ArmyTag&) {
            str.morale = std::min(1.0f, str.morale + 0.05f);
            str.organization = std::min(1.0f, str.organization + 0.03f);
        });
}

// ---------------------------------------------------------------------------
// Nation systems
// ---------------------------------------------------------------------------

void register_nation_systems(flecs::world& ecs) {
    // Monthly treasury update: aggregate province incomes for each nation.
    ecs.system<Treasury, NationManpower, const Diplomacy, const NationTag>(
            "NationMonthlyUpdate")
        .interval(30.0f)
        .each([&ecs](Treasury& treasury, NationManpower& nmp,
                     const Diplomacy& diplo, const NationTag& ntag) {
            float total_tax = 0.0f;
            float total_production = 0.0f;
            float total_trade = 0.0f;
            float total_max_manpower = 0.0f;

            // Sum over all provinces owned by this nation
            ecs.each([&](const ProvinceTag&, const ProvinceOwner& owner,
                         const Economy& econ, const Military& mil) {
                if (owner.nation_id == ntag.id) {
                    total_tax += econ.tax_income;
                    total_production += econ.production;
                    total_trade += econ.trade_value;
                    total_max_manpower += mil.supply_limit * 0.1f;
                }
            });

            treasury.monthly_income =
                total_tax + total_production * 0.3f + total_trade * 0.2f;

            // Simple expense model: stability cost + army maintenance
            float army_maintenance = 0.0f;
            ecs.each([&](const ArmyOwner& ao, const ArmyStrength& str) {
                if (ao.nation_id == ntag.id) {
                    army_maintenance += static_cast<float>(
                        str.infantry + str.cavalry * 2 +
                        str.artillery * 3) * 0.001f;
                }
            });

            float stability_cost =
                (diplo.stability < 0.0f) ? -diplo.stability * 0.5f : 0.0f;

            treasury.monthly_expenses = army_maintenance + stability_cost;

            // Apply monthly balance
            treasury.gold += treasury.monthly_income - treasury.monthly_expenses;

            // Nation manpower recovery
            // monthly_recovery = max_manpower * 0.01 * (1 - war_exhaustion/20)
            nmp.max_manpower = total_max_manpower;
            float we_factor =
                1.0f - std::clamp(diplo.war_exhaustion / 20.0f, 0.0f, 1.0f);
            nmp.monthly_recovery = nmp.max_manpower * 0.01f * we_factor;
            nmp.available =
                std::min(nmp.available + nmp.monthly_recovery,
                         nmp.max_manpower);
        });
}

}  // namespace systems
}  // namespace fenrir
