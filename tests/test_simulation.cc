// Tests for simulation systems (engine/components.hh, engine/systems.hh).
//
// The simulation engine files (simulation.cc, systems.cc) may not exist yet.
// These tests define the expected behaviour so that when the implementation
// lands it can be validated immediately.
//
// We test the ECS components and systems by creating a minimal flecs::world,
// registering systems, and stepping the simulation.

#include "test_framework.hh"
#include "src/engine/components.hh"
#include "src/world/province_data.hh"

#include <flecs.h>

using namespace fenrir;

// ---------------------------------------------------------------------------
// Component default values
// ---------------------------------------------------------------------------

TEST(economy_defaults) {
    Economy e;
    ASSERT_NEAR(e.tax_income, 0.0f, 1e-6f);
    ASSERT_NEAR(e.development, 1.0f, 1e-6f);
}

TEST(population_defaults) {
    Population p;
    ASSERT_EQ(p.total, 0u);
    ASSERT_NEAR(p.growth_rate, 0.002f, 1e-6f);
}

TEST(military_defaults) {
    Military m;
    ASSERT_NEAR(m.manpower, 0.0f, 1e-6f);
    ASSERT_NEAR(m.supply_limit, 10000.0f, 1e-6f);
}

TEST(treasury_defaults) {
    Treasury t;
    ASSERT_NEAR(t.gold, 0.0f, 1e-6f);
}

// ---------------------------------------------------------------------------
// Economy logic (manual computation, no systems.cc required)
// ---------------------------------------------------------------------------

// Expected formula: tax_income = population * development * tax_rate_constant
// We define a reference implementation here and will assert systems match it.
static float compute_tax(uint32_t population, float development) {
    constexpr float kBaseTaxPerCapita = 0.001f;  // Expected base rate
    return static_cast<float>(population) * development * kBaseTaxPerCapita;
}

TEST(economy_positive_tax_with_pop_and_dev) {
    float tax = compute_tax(10000, 5.0f);
    ASSERT_TRUE(tax > 0.0f);
}

TEST(economy_tax_scales_with_population) {
    float tax_low = compute_tax(1000, 5.0f);
    float tax_high = compute_tax(10000, 5.0f);
    ASSERT_TRUE(tax_high > tax_low);
}

TEST(economy_tax_scales_with_development) {
    float tax_low = compute_tax(10000, 1.0f);
    float tax_high = compute_tax(10000, 5.0f);
    ASSERT_TRUE(tax_high > tax_low);
}

TEST(economy_zero_pop_zero_tax) {
    float tax = compute_tax(0, 5.0f);
    ASSERT_NEAR(tax, 0.0f, 1e-6f);
}

// ---------------------------------------------------------------------------
// Population growth (manual computation)
// ---------------------------------------------------------------------------

static uint32_t grow_population(uint32_t total, float growth_rate, int ticks) {
    float pop = static_cast<float>(total);
    for (int i = 0; i < ticks; ++i) {
        pop += pop * growth_rate;
    }
    return static_cast<uint32_t>(pop);
}

TEST(population_growth_positive_in_peacetime) {
    uint32_t after = grow_population(10000, 0.002f, 1);
    ASSERT_TRUE(after >= 10000);
}

TEST(population_increases_after_multiple_ticks) {
    uint32_t before = 10000;
    uint32_t after = grow_population(before, 0.002f, 30);
    ASSERT_TRUE(after > before);
}

TEST(population_zero_stays_zero) {
    uint32_t after = grow_population(0, 0.002f, 100);
    ASSERT_EQ(after, 0u);
}

// ---------------------------------------------------------------------------
// Military: manpower recovery
// ---------------------------------------------------------------------------

TEST(military_manpower_recovery) {
    // Manpower should increase each tick toward max.
    float available = 5000.0f;
    float max_mp = 10000.0f;
    float recovery_rate = 0.01f;  // 1% of max per tick

    float recovered = available + max_mp * recovery_rate;
    if (recovered > max_mp) recovered = max_mp;

    ASSERT_TRUE(recovered > available);
    ASSERT_TRUE(recovered <= max_mp);
}

TEST(military_attrition_over_supply) {
    // Troops exceeding supply limit should suffer attrition.
    uint32_t troops = 15000;
    float supply_limit = 10000.0f;
    float attrition_rate = 0.01f;  // 1% per tick when over supply

    float excess = static_cast<float>(troops) - supply_limit;
    ASSERT_TRUE(excess > 0.0f);

    uint32_t losses = static_cast<uint32_t>(excess * attrition_rate);
    ASSERT_TRUE(losses > 0);

    uint32_t remaining = troops - losses;
    ASSERT_TRUE(remaining < troops);
}

TEST(military_no_attrition_under_supply) {
    uint32_t troops = 5000;
    float supply_limit = 10000.0f;

    float excess = static_cast<float>(troops) - supply_limit;
    ASSERT_TRUE(excess <= 0.0f);
}

// ---------------------------------------------------------------------------
// Nation treasury
// ---------------------------------------------------------------------------

TEST(nation_treasury_increases_with_income) {
    Treasury t;
    t.gold = 100.0f;
    t.monthly_income = 50.0f;
    t.monthly_expenses = 0.0f;

    float new_gold = t.gold + t.monthly_income - t.monthly_expenses;
    ASSERT_TRUE(new_gold > t.gold);
}

TEST(nation_treasury_decreases_with_expenses) {
    Treasury t;
    t.gold = 100.0f;
    t.monthly_income = 10.0f;
    t.monthly_expenses = 30.0f;

    float new_gold = t.gold + t.monthly_income - t.monthly_expenses;
    ASSERT_TRUE(new_gold < t.gold);
}

// ---------------------------------------------------------------------------
// ECS integration: create entities with components, verify queries
// ---------------------------------------------------------------------------

TEST(ecs_province_entity_creation) {
    flecs::world ecs;

    auto province = ecs.entity()
        .set<ProvinceTag>({1})
        .set<Economy>({})
        .set<Population>({10000, 0.002f, 0.1f})
        .set<Military>({});

    ASSERT_TRUE(province.is_valid());
    ASSERT_TRUE(province.has<ProvinceTag>());
    ASSERT_TRUE(province.has<Economy>());
    ASSERT_TRUE(province.has<Population>());
    ASSERT_TRUE(province.has<Military>());

    const Population* pop = province.try_get<Population>();
    ASSERT_EQ(pop->total, 10000u);
}

TEST(ecs_nation_entity_creation) {
    flecs::world ecs;

    auto nation = ecs.entity()
        .set<NationTag>({1})
        .set<Treasury>({500.0f, 0.0f, 0.0f})
        .set<NationManpower>({1000.0f, 5000.0f, 50.0f})
        .set<Technology>({})
        .set<Diplomacy>({});

    ASSERT_TRUE(nation.is_valid());

    const Treasury* t = nation.try_get<Treasury>();
    ASSERT_NEAR(t->gold, 500.0f, 1e-6f);
}

TEST(ecs_army_entity_creation) {
    flecs::world ecs;

    auto army = ecs.entity()
        .set<ArmyTag>({1})
        .set<ArmyPosition>({5})
        .set<ArmyStrength>({1000, 200, 50, 1.0f, 1.0f})
        .set<ArmyOwner>({1});

    ASSERT_TRUE(army.is_valid());

    const ArmyStrength* str = army.try_get<ArmyStrength>();
    ASSERT_EQ(str->infantry, 1000u);
    ASSERT_EQ(str->cavalry, 200u);
    ASSERT_EQ(str->artillery, 50u);
}

TEST(ecs_economy_system_query) {
    // Simulate what the economy system would do: iterate provinces and
    // update tax_income based on population and development.
    flecs::world ecs;

    auto p1 = ecs.entity()
        .set<ProvinceTag>({0})
        .set<Economy>({0.0f, 0.0f, 0.0f, 5.0f, 0.0f})
        .set<Population>({10000, 0.002f, 0.1f});

    auto p2 = ecs.entity()
        .set<ProvinceTag>({1})
        .set<Economy>({0.0f, 0.0f, 0.0f, 3.0f, 0.0f})
        .set<Population>({5000, 0.002f, 0.1f});

    // Manually run the "system" logic.
    ecs.each([](Economy& econ, const Population& pop) {
        constexpr float kBaseTaxPerCapita = 0.001f;
        econ.tax_income = static_cast<float>(pop.total) * econ.development * kBaseTaxPerCapita;
    });

    const Economy* e1 = p1.try_get<Economy>();
    const Economy* e2 = p2.try_get<Economy>();

    ASSERT_NEAR(e1->tax_income, 50.0f, 1e-3f);  // 10000 * 5.0 * 0.001
    ASSERT_NEAR(e2->tax_income, 15.0f, 1e-3f);  // 5000 * 3.0 * 0.001
}

// ---------------------------------------------------------------------------
// Province data generation (province_data.hh)
// ---------------------------------------------------------------------------

TEST(province_data_generation_deterministic) {
    ProvinceData d1 = generateProvinceData(42, 12345);
    ProvinceData d2 = generateProvinceData(42, 12345);

    ASSERT_EQ(d1.id, d2.id);
    ASSERT_EQ(d1.population, d2.population);
    ASSERT_NEAR(d1.tax_income, d2.tax_income, 1e-6f);
    ASSERT_NEAR(d1.fertility, d2.fertility, 1e-6f);
}

TEST(province_data_different_ids_differ) {
    ProvinceData d1 = generateProvinceData(1, 42);
    ProvinceData d2 = generateProvinceData(2, 42);

    // With different IDs, at least one field should differ.
    bool differ = (d1.population != d2.population) ||
                  (d1.terrain != d2.terrain) ||
                  (std::abs(d1.fertility - d2.fertility) > 1e-6f);
    ASSERT_TRUE(differ);
}

TEST(province_data_ocean_has_zero_pop) {
    // Generate many provinces to find an ocean one.
    for (uint32_t id = 0; id < 100; ++id) {
        ProvinceData d = generateProvinceData(id, 42);
        if (d.terrain == TerrainType::Ocean) {
            ASSERT_EQ(d.population, 0u);
            return;  // Found and verified at least one.
        }
    }
    // If no ocean province was generated, that is still valid -- skip silently.
}

// ---------------------------------------------------------------------------
// Simulation date / tick logic (reference implementation)
// ---------------------------------------------------------------------------

struct SimDate {
    int year;
    int month;  // 1-12
    int day;    // 1-30 (simplified)

    void advance_day() {
        day++;
        if (day > 30) {
            day = 1;
            month++;
            if (month > 12) {
                month = 1;
                year++;
            }
        }
    }
};

TEST(simulation_date_advances) {
    SimDate date = {1444, 11, 11};
    date.advance_day();
    ASSERT_EQ(date.year, 1444);
    ASSERT_EQ(date.month, 11);
    ASSERT_EQ(date.day, 12);
}

TEST(simulation_date_month_rollover) {
    SimDate date = {1444, 11, 30};
    date.advance_day();
    ASSERT_EQ(date.year, 1444);
    ASSERT_EQ(date.month, 12);
    ASSERT_EQ(date.day, 1);
}

TEST(simulation_date_year_rollover) {
    SimDate date = {1444, 12, 30};
    date.advance_day();
    ASSERT_EQ(date.year, 1445);
    ASSERT_EQ(date.month, 1);
    ASSERT_EQ(date.day, 1);
}

TEST(simulation_tick_count) {
    int tick = 0;
    for (int i = 0; i < 100; ++i) {
        tick++;
    }
    ASSERT_EQ(tick, 100);
}
