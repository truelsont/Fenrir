# Architecture Overview

## System Layers

```
Raylib + ImGui (Rendering/UI/Input)
    ↓ reads state, sends commands
Main Loop (C++ Application)
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
- **Raylib 5.0+**: Rendering, input, window management
- **Dear ImGui**: UI overlays and debug panels
- **CMake**: Build system
- **Boost 1.85.0**: Logging, graphs, random (vendored)
- **LibTorch**: Agent ML (future)
- **Vulkan/CUDA/Metal**: GPU compute (future)

## Module Structure

```
fenrir (executable) → Main application with Raylib
    ├── src/main.cc           → Application entry point
    ├── src/ui/               → Rendering and UI layer
    ├── src/state/            → World state management
    ├── src/engine/           → Simulation engines
    ├── src/compute/          → Compute facade + backends
    └── src/util/             → Utilities (logging, threading)
```

## Data Flow

### Simulation Tick
1. **Main Loop** calls `StateManager.tick(delta)` each frame
2. **StateManager** executes queued player commands
3. **StateManager** calls each engine's `tick(WorldState&, delta)`
4. **Engines** mutate WorldState, submit tasks to Compute Facade
5. **Compute Facade** executes heavy tasks (CPU/GPU)
6. **StateManager** increments game time
7. **Renderer** reads WorldState (const) for rendering
8. **ImGui** displays UI overlays with WorldState data

### Player Actions
1. **Player** clicks button or presses key
2. **InputHandler** detects input event
3. **Main Loop** creates Command, queues in StateManager
4. **Command** executes on next tick (mutates WorldState)

### Agent Learning
1. **Agent** requests observation via IDataProvider
2. **IDataProvider** reads WorldState (const)
3. **Agent** makes decision based on observation
4. **Agent** queues action as Command (if needed)