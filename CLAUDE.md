# Fenrir

A high-performance grand strategy game engine with granular economic simulation, agent-based learning, and multi-scale world representation (orbital to ground level).

## Vision

Fenrir simulates a living world from space down to individual provinces. Players and AI agents interact with interconnected economic, political, cultural, and military systems. The engine supports both gameplay and ML agent training on the same simulation.

**Inspirations**: Victoria 2 (POPs, economy), Dwarf Fortress (simulation depth), Kerbal Space Program (multi-scale physics)

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     Raylib + ImGui                          │
│                  (Rendering, Input, UI)                     │
└─────────────────────────┬───────────────────────────────────┘
                          │
┌─────────────────────────▼───────────────────────────────────┐
│                      Main Loop                              │
│              (Frame timing, coordination)                   │
└─────────────────────────┬───────────────────────────────────┘
                          │
┌─────────────────────────▼───────────────────────────────────┐
│                    State Manager                            │
│         (Commands, tick coordination, save/load)            │
└───────┬─────────┬─────────┬─────────┬───────────────────────┘
        │         │         │         │
   ┌────▼───┐ ┌───▼───┐ ┌───▼───┐ ┌───▼────┐
   │Economy │ │Politics│ │Culture│ │Military│  ← Simulation Engines
   └────┬───┘ └───┬───┘ └───┬───┘ └───┬────┘
        │         │         │         │
┌───────▼─────────▼─────────▼─────────▼───────────────────────┐
│                      Flecs ECS                              │
│            (Entities, Components, Systems)                  │
└─────────────────────────┬───────────────────────────────────┘
                          │
┌─────────────────────────▼───────────────────────────────────┐
│                   Compute Facade                            │
│              (Task scheduling, CPU/GPU)                     │
└─────────────────────────────────────────────────────────────┘
```

## Core Concepts

### Multi-Scale World (3D Globe)

The world is a sphere with multiple zoom levels:

1. **Orbital** - Satellites, space stations, orbital mechanics
2. **Continental** - Globe view, weather systems, ocean currents
3. **Regional** - Countries, major geographic features
4. **Provincial** - Individual provinces, cities, terrain
5. **Local** - Street level (future)

At planetary zoom levels, the surface appears "near-flat" using appropriate map projections. Entities exist at all scales (satellites in orbit, ships at sea, armies on land).

### Entity Component System (Flecs)

All game objects are entities with composable components:

```cpp
// Entities are just IDs
flecs::entity province = world.entity()
    .set<Position>({45.0f, -122.0f})        // Lat/lon on globe
    .set<Province>({area_id, owner_id})
    .set<Economy>({gdp, tax_rate})
    .set<Terrain>({TerrainType::Plains});

// Systems operate on component queries
world.system<Economy, const Population>()
    .each([](Economy& econ, const Population& pop) {
        econ.labor_supply = pop.working_age * pop.employment_rate;
    });
```

### Serialization (FlatBuffers)

All entity data uses FlatBuffers schemas for:
- Zero-copy reads (fast save/load)
- Network sync (multiplayer-ready)
- Language-agnostic (Python ML tools can read game state)
- Schema evolution (add fields without breaking saves)

Schemas live in `schemas/` directory.

### Data Abstraction Layer

Agents (AI or ML) observe the world through `IDataProvider`, never accessing raw state:

```cpp
class IDataProvider {
    virtual MarketSnapshot getMarketData(ProvinceId id) = 0;
    virtual AgentObservation getObservation(AgentId id) = 0;
};
```

This enables:
- Training on simulated or real-world data
- Observation noise/fog of war
- Reproducible agent behavior

## Directory Structure

```
fenrir/
├── src/
│   ├── main.cc                 # Entry point
│   ├── ui/                     # Rendering, input, overlays
│   ├── state/                  # World state, ECS world
│   ├── engine/                 # Simulation engines
│   ├── compute/                # CPU/GPU task execution
│   └── util/                   # Logging, data structures
├── schemas/                    # FlatBuffers schemas
├── external/                   # Git submodules (raylib, flecs, boost, imgui)
├── docs/                       # Architecture, API docs
└── tests/                      # Unit and integration tests
```

## Code Style

- **C++17** standard
- **Google C++ Style** (`.hh` headers, `.cc` implementations)
- **No raw `new/delete`** - use smart pointers
- **Prefer composition** over inheritance
- **Data-oriented design** - SoA over AoS where it matters
- **Deterministic** - seeded RNG, fixed-point for reproducibility

## Building

```bash
# Clone with submodules
git clone --recursive https://github.com/user/fenrir.git
cd fenrir

# Build
./build.sh

# Run
./bin/fenrir
```

## Key Dependencies

| Library | Purpose | Location |
|---------|---------|----------|
| Raylib 5.0 | Rendering, input, window | `external/raylib` |
| Dear ImGui | Debug UI, overlays | `external/imgui` |
| Flecs | Entity Component System | `external/flecs` |
| Boost 1.85 | Logging, utilities | `external/boost` |
| FlatBuffers | Serialization schemas | `external/flatbuffers` |

## Development Priorities

1. **ECS foundation** - Migrate entities to Flecs
2. **3D globe** - Sphere geometry with multi-scale zoom
3. **FlatBuffers schemas** - Define core entity schemas
4. **Economy simulation** - Markets, production, trade
5. **Agent learning** - ML agents via LibTorch

## Anti-Patterns to Avoid

- **God objects** - Split large classes into components
- **Inheritance hierarchies** - Use ECS composition
- **String keys** - Use typed IDs and enums
- **Blocking I/O in game loop** - Use async/compute facade
- **Raw pointers for ownership** - Use `unique_ptr`/`shared_ptr`
