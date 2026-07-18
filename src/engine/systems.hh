#pragma once

#include <flecs.h>

namespace fenrir {
namespace systems {

// Economy: provinces generate tax based on population and development.
// Runs every tick (1 day). Monthly aggregation done in nation systems.
void register_economy_systems(flecs::world& ecs);

// Population: growth based on fertility, development, war impact.
// Growth applied monthly.
void register_population_systems(flecs::world& ecs);

// Military: manpower recovery, attrition for over-supply armies, morale
// recovery when not in combat.
void register_military_systems(flecs::world& ecs);

// Nation: aggregate province incomes, update treasury each month,
// recover manpower.
void register_nation_systems(flecs::world& ecs);

}  // namespace systems
}  // namespace fenrir
