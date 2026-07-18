#pragma once

#include <flecs.h>
#include <cstdint>
#include <vector>

#include "engine/components.hh"

namespace fenrir {

// Holds the full simulation state and drives the Flecs ECS world forward.
// Province and nation data is generated deterministically from seed values.
class Simulation {
 public:
    Simulation();

    // Create province and nation entities, register all systems.
    // num_provinces: total land provinces (ocean excluded)
    // num_nations:   number of playable nations
    void initialize(uint32_t num_provinces, uint32_t num_nations);

    // Advance the simulation clock by |delta_days|.
    // Internally this accumulates fractional days and calls ecs_.progress()
    // once per whole day.
    void tick(float delta_days);

    flecs::world& world() { return ecs_; }
    const flecs::world& world() const { return ecs_; }

    // Lookup helpers (return nullptr if not found).
    const Economy* getProvinceEconomy(uint32_t province_id) const;
    const Population* getProvincePopulation(uint32_t province_id) const;
    const Military* getProvinceMilitary(uint32_t province_id) const;
    const ProvinceOwner* getProvinceOwner(uint32_t province_id) const;

    const Treasury* getNationTreasury(uint32_t nation_id) const;
    const NationManpower* getNationManpower(uint32_t nation_id) const;
    const Technology* getNationTechnology(uint32_t nation_id) const;
    const Diplomacy* getNationDiplomacy(uint32_t nation_id) const;

    // Calendar helpers.
    uint32_t current_day() const;
    uint32_t current_month() const;  // 1-12
    uint32_t current_year() const;

    uint32_t tick_count() const { return tick_count_; }

 private:
    // Deterministic seed used during initialization.
    static constexpr uint64_t kSeed = 42;

    flecs::world ecs_;
    float accumulated_days_ = 0.0f;
    uint32_t tick_count_ = 0;          // Total whole-day ticks elapsed
    uint32_t start_year_ = 1444;       // EU4-style start date

    // Entity lookup caches (indexed by id).
    std::vector<flecs::entity> province_entities_;
    std::vector<flecs::entity> nation_entities_;

    void create_provinces(uint32_t count);
    void create_nations(uint32_t count);
    void assign_provinces_to_nations();
    void register_systems();
};

}  // namespace fenrir
