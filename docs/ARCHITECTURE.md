# Architecture Overview

## System Layers

```
Godot (UI/Rendering/Input)
    ↓ reads state, sends commands
GameNode (C++ GDExtension Bridge)
    ↓ coordinates
StateManager (Central Coordinator)
    ├── owns → WorldState (Game Data)
    ├── coordinates → Simulation Engines
    └── executes → Player Commands
    ↓
Simulation Engines (Economy, Politics, Culture, Military)
    ↓ use
Compute Facade (Task-based execution)
    ↓
CPU/GPU Backends

Data Abstraction Layer (IDataProvider)
    ↓ reads from
WorldState (for agents/ML)
```

## Core Principles

- **Data Abstraction**: Agents learn from observations, not direct simulation access
- **Compute Facade**: All heavy computation goes through async task system
- **Source Agnostic**: Same code handles simulated and real-world data
- **Deterministic**: Seeded RNG, fixed-point math for reproducibility
- **Digital Twin**: Validate simulation against real-world data

## Tech Stack

- **C++17**: Core engine
- **Godot 4.4+**: Frontend
- **CMake**: Build system
- **Boost**: Logging, graphs, random
- **LibTorch**: Agent ML (future)
- **Vulkan/CUDA/Metal**: GPU compute (future)

## Module Structure

```
libfenrir_util      → Utilities (logging, threading)
libfenrir_compute   → Compute facade + backends
libfenrir_game      → Data models, abstractions
libfenrir_engine    → Simulation engines
libfenrir_godot.so  → Godot GDExtension
```

## Data Flow

### Simulation Tick
1. **Godot** calls `GameNode._process(delta)`
2. **GameNode** calls `StateManager.tick(delta)`
3. **StateManager** executes queued player commands
4. **StateManager** calls each engine's `tick(WorldState&, delta)`
5. **Engines** mutate WorldState, submit tasks to Compute Facade
6. **Compute Facade** executes heavy tasks (CPU/GPU)
7. **StateManager** increments game time
8. **Godot** reads WorldState (const) for rendering

### Player Actions
1. **Player** clicks button in Godot UI
2. **GDScript** calls `GameNode.setTaxRate(nation_id, rate)`
3. **GameNode** creates Command, queues in StateManager
4. **Command** executes on next tick (mutates WorldState)

### Agent Learning
1. **Agent** requests observation via IDataProvider
2. **IDataProvider** reads WorldState (const)
3. **Agent** makes decision based on observation
4. **Agent** queues action as Command (if needed)