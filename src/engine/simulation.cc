#include "engine/simulation.hh"
#include "engine/systems.hh"
#include "util/logger/logger.hh"

#include <algorithm>
#include <cmath>
#include <string>

namespace fenrir {

// -----------------------------------------------------------------------
// Deterministic RNG helper (same as province_data.hh style)
// -----------------------------------------------------------------------
namespace {

class DetRng {
 public:
    explicit DetRng(uint64_t seed) : state_(seed) {}

    // Returns a float in [0, 1).
    float next() {
        state_ = state_ * 6364136223846793005ULL + 1442695040888963407ULL;
        return static_cast<float>(state_ >> 33) /
               static_cast<float>(1ULL << 31);
    }

    // Returns an integer in [lo, hi] inclusive.
    uint32_t next_int(uint32_t lo, uint32_t hi) {
        return lo + static_cast<uint32_t>(next() * static_cast<float>(hi - lo + 1));
    }

 private:
    uint64_t state_;
};

Logger& sim_logger() {
    static Logger logger;
    return logger;
}

}  // namespace

// -----------------------------------------------------------------------
// Construction
// -----------------------------------------------------------------------

Simulation::Simulation() {
    sim_logger().SetLevel(LogLevel::kDebug);
}

// -----------------------------------------------------------------------
// Initialization
// -----------------------------------------------------------------------

void Simulation::initialize(uint32_t num_provinces, uint32_t num_nations) {
    sim_logger().Info("Simulation::initialize  provinces=" +
                      std::to_string(num_provinces) +
                      "  nations=" + std::to_string(num_nations));

    create_provinces(num_provinces);
    create_nations(num_nations);
    assign_provinces_to_nations();
    register_systems();

    sim_logger().Info("Simulation initialized. Start year: " +
                      std::to_string(start_year_));
}

// -----------------------------------------------------------------------
// Tick
// -----------------------------------------------------------------------

void Simulation::tick(float delta_days) {
    accumulated_days_ += delta_days;

    // Process whole-day increments.  Each ecs_.progress(1.0) represents
    // one in-game day.  Systems that use interval(30) trigger monthly.
    while (accumulated_days_ >= 1.0f) {
        accumulated_days_ -= 1.0f;
        ecs_.progress(1.0f);
        tick_count_++;

        // Yearly log
        if (tick_count_ % 360 == 0) {
            sim_logger().Debug("Simulation year " +
                               std::to_string(current_year()) +
                               " reached (tick " +
                               std::to_string(tick_count_) + ")");
        }
    }
}

// -----------------------------------------------------------------------
// Province creation
// -----------------------------------------------------------------------

void Simulation::create_provinces(uint32_t count) {
    province_entities_.resize(count);
    DetRng rng(kSeed);

    for (uint32_t i = 0; i < count; ++i) {
        // Deterministic province stats
        float fertility = 0.3f + rng.next() * 0.7f;   // 0.3 - 1.0
        float resources = rng.next();  // Consumed to keep RNG sequence stable
        (void)resources;
        uint32_t pop = static_cast<uint32_t>(
            10000.0f + fertility * 990000.0f * rng.next());
        float dev = 1.0f + rng.next() * 4.0f;  // 1.0 - 5.0 initial

        auto e = ecs_.entity()
            .set<ProvinceTag>({i})
            .set<Economy>({
                /* tax_income       */ 0.0f,
                /* production       */ 0.0f,
                /* trade_value      */ 0.0f,
                /* development      */ dev,
                /* local_trade_power*/ rng.next() * 2.0f
            })
            .set<Population>({
                /* total       */ pop,
                /* growth_rate */ 0.002f,
                /* literacy    */ 0.05f + rng.next() * 0.15f
            })
            .set<Military>({
                /* manpower       */ static_cast<float>(pop) * 0.05f,
                /* fortification  */ rng.next() * 3.0f,
                /* supply_limit   */ static_cast<float>(pop) * 0.1f
            })
            .set<ProvinceOwner>({0, 0});  // unowned initially

        province_entities_[i] = e;
    }

    sim_logger().Debug("Created " + std::to_string(count) + " provinces");
}

// -----------------------------------------------------------------------
// Nation creation
// -----------------------------------------------------------------------

void Simulation::create_nations(uint32_t count) {
    nation_entities_.resize(count);
    DetRng rng(kSeed ^ 0xDEADBEEF);

    for (uint32_t i = 0; i < count; ++i) {
        float starting_gold = 50.0f + rng.next() * 200.0f;

        auto e = ecs_.entity()
            .set<NationTag>({i})
            .set<Treasury>({
                /* gold            */ starting_gold,
                /* monthly_income  */ 0.0f,
                /* monthly_expenses*/ 0.0f
            })
            .set<NationManpower>({
                /* available        */ 0.0f,
                /* max_manpower     */ 0.0f,
                /* monthly_recovery */ 0.0f
            })
            .set<Technology>({
                /* admin_tech   */ 1.0f + rng.next() * 2.0f,
                /* diplo_tech   */ 1.0f + rng.next() * 2.0f,
                /* military_tech*/ 1.0f + rng.next() * 2.0f
            })
            .set<Diplomacy>({
                /* prestige       */ rng.next() * 20.0f - 10.0f,
                /* stability      */ 1.0f,
                /* war_exhaustion */ 0.0f
            });

        nation_entities_[i] = e;
    }

    sim_logger().Debug("Created " + std::to_string(count) + " nations");
}

// -----------------------------------------------------------------------
// Province-to-nation assignment
// -----------------------------------------------------------------------
// Divides provinces roughly equally.  With globe data available, this
// could use geographic proximity (Voronoi on the sphere); for now a
// simple round-robin provides an even, deterministic split.

void Simulation::assign_provinces_to_nations() {
    if (nation_entities_.empty()) return;

    uint32_t num_nations = static_cast<uint32_t>(nation_entities_.size());

    for (uint32_t i = 0; i < province_entities_.size(); ++i) {
        uint32_t owner = i % num_nations;
        province_entities_[i].set<ProvinceOwner>({owner, owner});
    }

    sim_logger().Debug("Assigned " +
                       std::to_string(province_entities_.size()) +
                       " provinces to " + std::to_string(num_nations) +
                       " nations (round-robin)");
}

// -----------------------------------------------------------------------
// System registration
// -----------------------------------------------------------------------

void Simulation::register_systems() {
    systems::register_economy_systems(ecs_);
    systems::register_population_systems(ecs_);
    systems::register_military_systems(ecs_);
    systems::register_nation_systems(ecs_);
    sim_logger().Debug("All simulation systems registered");
}

// -----------------------------------------------------------------------
// Query helpers
// -----------------------------------------------------------------------

const Economy* Simulation::getProvinceEconomy(uint32_t province_id) const {
    if (province_id >= province_entities_.size()) return nullptr;
    return province_entities_[province_id].try_get<Economy>();
}

const Population* Simulation::getProvincePopulation(uint32_t province_id) const {
    if (province_id >= province_entities_.size()) return nullptr;
    return province_entities_[province_id].try_get<Population>();
}

const Military* Simulation::getProvinceMilitary(uint32_t province_id) const {
    if (province_id >= province_entities_.size()) return nullptr;
    return province_entities_[province_id].try_get<Military>();
}

const ProvinceOwner* Simulation::getProvinceOwner(uint32_t province_id) const {
    if (province_id >= province_entities_.size()) return nullptr;
    return province_entities_[province_id].try_get<ProvinceOwner>();
}

const Treasury* Simulation::getNationTreasury(uint32_t nation_id) const {
    if (nation_id >= nation_entities_.size()) return nullptr;
    return nation_entities_[nation_id].try_get<Treasury>();
}

const NationManpower* Simulation::getNationManpower(uint32_t nation_id) const {
    if (nation_id >= nation_entities_.size()) return nullptr;
    return nation_entities_[nation_id].try_get<NationManpower>();
}

const Technology* Simulation::getNationTechnology(uint32_t nation_id) const {
    if (nation_id >= nation_entities_.size()) return nullptr;
    return nation_entities_[nation_id].try_get<Technology>();
}

const Diplomacy* Simulation::getNationDiplomacy(uint32_t nation_id) const {
    if (nation_id >= nation_entities_.size()) return nullptr;
    return nation_entities_[nation_id].try_get<Diplomacy>();
}

// -----------------------------------------------------------------------
// Calendar
// -----------------------------------------------------------------------
// Simple calendar: 360 days/year, 30 days/month, 12 months.
// No leap years for determinism.

uint32_t Simulation::current_day() const {
    return (tick_count_ % 30) + 1;
}

uint32_t Simulation::current_month() const {
    return ((tick_count_ / 30) % 12) + 1;
}

uint32_t Simulation::current_year() const {
    return start_year_ + (tick_count_ / 360);
}

}  // namespace fenrir
