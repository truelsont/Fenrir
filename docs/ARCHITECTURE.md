# Architecture Overview

## System Layers

```
Raylib + ImGui (Rendering/UI/Input)
    ↓ reads state, sends commands
Main Loop (C++ Application)
    ↓ coordinates
StateManager (Central Coordinator)
    ├── owns → Flecs ECS World (All Entities)
    ├── coordinates → Simulation Engines
    └── executes → Player Commands
    ↓
Simulation Engines (Economy, Politics, Culture, Military)
    ↓ query/mutate entities
Flecs ECS (Entity Component System)
    ↓ use
Compute Facade (Task-based execution)
    ↓
CPU/GPU Backends

Data Abstraction Layer (IDataProvider)
    ↓ reads from
FlatBuffers Snapshots (for agents/ML/network)
```

## Core Principles

- **ECS-First**: All game objects are Flecs entities with composable components
- **Data Abstraction**: Agents learn from observations, not direct simulation access
- **Compute Facade**: All heavy computation goes through async task system
- **Schema-Driven**: FlatBuffers schemas define all entity data for serialization/network
- **Multi-Scale World**: Seamless zoom from orbital to ground level
- **Deterministic**: Seeded RNG, fixed-point math for reproducibility

## Multi-Scale World Model

The world is a 3D sphere with entities existing at multiple scales:

```
ORBITAL (altitude > 100km)
    │   Satellites, space stations, orbital mechanics
    │   Rendered as points/icons on globe
    │
CONTINENTAL (globe view)
    │   Weather systems, ocean currents, day/night
    │   Sphere geometry with texture mapping
    │
REGIONAL (country level)
    │   Nations, major geographic features
    │   Mercator-like projection, curved horizon
    │
PROVINCIAL (zoomed in)
    │   Individual provinces, cities, terrain
    │   Near-flat projection, detailed terrain
    │
LOCAL (street level - future)
        Buildings, people, vehicles
        Full 3D scene
```

### Coordinate Systems

- **Orbital frame**: Cartesian (x, y, z) centered on planet
- **Geographic**: Latitude/longitude/altitude (GeoCoord)
- **Screen**: 2D pixel coordinates after projection

### Projection

At high zoom (provincial/local), use **transverse Mercator** projection centered on camera to achieve "near-flat earth" appearance while maintaining correct global positioning.

## Entity Component System (Flecs)

### Why Flecs

- Cache-friendly archetype storage
- Built-in query caching
- Multi-threading support
- Reflection and serialization helpers
- Active development, good documentation

### Component Design

Components are pure data structs. No methods, no inheritance:

```cpp
// Identity
struct ProvinceTag {};
struct NationTag {};
struct PopTag {};
struct OrbitalTag {};

// Spatial
struct GeoPosition { float lat, lon, alt; };
struct OrbitalPosition { float x, y, z; };
struct Velocity { float vx, vy, vz; };

// Geographic hierarchy
struct InProvince { flecs::entity province; };
struct InArea { flecs::entity area; };
struct InRegion { flecs::entity region; };
struct Ownership { flecs::entity nation; };

// Simulation data
struct Economy { float gdp, tax_rate, trade_value; };
struct Population { uint32_t size; float growth_rate; };
struct Military { uint32_t army_size; float morale; };
struct Terrain { TerrainType type; };
struct Resources { std::vector<ResourceType> types; };
```

### Systems

Systems are functions that operate on component queries:

```cpp
// Economy system - runs each tick
world.system<Economy, const Population, const Resources>()
    .each([](flecs::entity e, Economy& econ,
             const Population& pop, const Resources& res) {
        econ.gdp = calculateGDP(pop, res);
    });

// Orbital mechanics - updates satellite positions
world.system<OrbitalPosition, Velocity, const OrbitalParameters>()
    .each([](OrbitalPosition& pos, Velocity& vel,
             const OrbitalParameters& orbit) {
        updateKeplerianOrbit(pos, vel, orbit, delta_time);
    });
```

## Serialization (FlatBuffers)

### Why FlatBuffers

- Zero-copy access (no parsing overhead)
- Schema evolution (add fields without breaking saves)
- Cross-language (Python ML tools read game state)
- Compact binary format
- gRPC compatible for future multiplayer

### Schema Location

All schemas in `schemas/` directory:
- `world.fbs` - Core entity definitions
- `market.fbs` - Economic data (future)
- `agent.fbs` - ML observation format (future)

### Usage Pattern

```cpp
// Serialize world state
flatbuffers::FlatBufferBuilder builder;
auto provinces = serializeProvinces(world, builder);
auto state = CreateWorldState(builder, tick, seed, provinces);
builder.Finish(state);
saveToFile(builder.GetBufferPointer(), builder.GetSize());

// Deserialize
auto buffer = loadFromFile("save.bin");
auto state = GetWorldState(buffer.data());
for (auto province : *state->provinces()) {
    world.entity()
        .set<Province>({province->id(), province->area_id()})
        .set<GeoPosition>({province->center()->latitude(), ...});
}
```

## Data Flow

### Simulation Tick

1. **Main Loop** calls `StateManager.tick(delta)` each frame
2. **StateManager** executes queued player commands
3. **Flecs** runs registered systems in dependency order:
   - Population growth
   - Economic production
   - Market clearing
   - Political events
   - Military movement
4. **Compute Facade** executes heavy parallel tasks (pathfinding, market clearing)
5. **StateManager** increments game time
6. **Renderer** queries Flecs world for visible entities
7. **ImGui** displays UI overlays

### Player Actions

1. **Player** clicks button or presses key
2. **InputHandler** detects input event
3. **Main Loop** creates Command entity with `CommandTag` component
4. **Command system** processes and removes command entities

### Agent Learning (ML)

1. **Agent** requests observation via `IDataProvider`
2. **IDataProvider** queries Flecs, builds FlatBuffer `AgentObservation`
3. **Agent** (LibTorch) processes observation tensor
4. **Agent** queues action as Command entity

## Tech Stack

| Component | Technology | Purpose |
|-----------|-----------|---------|
| Core Engine | C++17 | High-performance simulation |
| ECS | Flecs | Entity management, system scheduling |
| Serialization | FlatBuffers | Save/load, network, ML interface |
| Rendering | Raylib 5.0 | Window, input, texture rendering |
| UI Overlays | Dear ImGui | Debug panels, overlays |
| Build System | CMake 3.20+ | Cross-platform compilation |
| Logging | Boost.Log 1.85.0 | Async logging system |
| ML (future) | LibTorch | Agent training and inference |
| GPU (future) | Vulkan/Metal | Compute shaders for heavy simulation |

## Module Structure

```
fenrir/
├── src/
│   ├── main.cc                 # Application entry point
│   ├── ui/                     # Rendering and UI layer
│   │   ├── renderer.cc         # World texture rendering
│   │   ├── globe_renderer.cc   # 3D globe projection (future)
│   │   ├── input_handler.cc    # Camera and player input
│   │   └── ui_overlay.cc       # ImGui debug panels
│   ├── state/                  # World state management
│   │   ├── ecs_world.cc        # Flecs world setup
│   │   ├── components.hh       # All component definitions
│   │   └── systems/            # Flecs systems
│   ├── engine/                 # Simulation engines
│   │   ├── economy/            # Market, production, trade
│   │   ├── politics/           # Nations, diplomacy
│   │   ├── culture/            # POPs, migration
│   │   └── military/           # Armies, warfare
│   ├── compute/                # Compute facade + backends
│   │   ├── compute_facade.cc   # Task scheduling interface
│   │   ├── cpu_backend.cc      # Thread pool executor
│   │   └── gpu_backend.cc      # Vulkan compute (future)
│   └── util/                   # Utilities
├── schemas/                    # FlatBuffers schemas
├── external/                   # Dependencies (submodules)
└── tests/                      # Unit and integration tests
```

## Future: gRPC for Multiplayer

FlatBuffers + gRPC enables:

```protobuf
service FenrirGame {
    rpc JoinGame(JoinRequest) returns (JoinResponse);
    rpc SendCommand(Command) returns (CommandAck);
    rpc StreamState(StateRequest) returns (stream WorldDelta);
}
```

State deltas sent as FlatBuffers for minimal bandwidth and zero-copy processing.
