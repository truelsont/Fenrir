# Fenrir

Grand strategy game (Dwarf Fortress + Victoria 2 style) with granular economic simulation and agent-based learning.

## Core Features

- **Economic Simulation**: Markets, trade, production chains, agent-based
- **Political System**: Nations, diplomacy, governance, taxation
- **Cultural System**: POPs, culture spread, migration
- **Military System**: Armies, warfare, supply logistics
- **Map Generation**: Procedural terrain, resources, provinces
- **Digital Twin**: Train agents on real-world data, test in simulation

## Tech Stack

- **C++17** + **Godot 4.4** + **CMake**
- **Compute Facade**: Task-based (CPU/GPU/ML)
- **Data Abstraction**: Simulated + real-world sources
- **Future**: Boost, LibTorch, Vulkan/CUDA/Metal

## Quick Start

```bash
# macOS: brew install cmake boost
# Linux: sudo apt-get install cmake libboost-all-dev

chmod +x build.sh
./build.sh
```

The build script will:
- Auto-detect your platform (macOS/Linux)
- Check/install dependencies
- Build everything
- Show platform-specific run instructions

## Documentation

- **[Setup Guide](docs/SETUP.md)** - Installation and build instructions
- **[Architecture](docs/ARCHITECTURE.md)** - System design and principles
- **[Style Guide](docs/STYLE_GUIDE.md)** - C++ coding standards (Google style)
- **[TODO](TODO.md)** - Development roadmap and tasks

## Project Structure

```
fenrir/
├── src/                # C++ engine code (.hh/.cc, Google style)
│   └── godot/          # Godot integration layer
├── godot_project/      # Godot project files
├── bin/                # Compiled libraries
├── external/           # Dependencies (godot-cpp, cmake)
├── docs/               # Documentation
│   ├── SETUP.md        # Build instructions
│   ├── ARCHITECTURE.md # System design
│   └── STYLE_GUIDE.md  # C++ coding standards
└── build.sh            # Automated build script
```

## Development

**Code Style**: Google C++ Style Guide  
**File Extensions**: `.hh` for headers, `.cc` for implementation  
**Formatting**: Run `clang-format -i <file>` before committing  
**Platforms**: macOS (primary), Linux (supported)  
**LLM Agents**: Read `llm-system-prompts/` before starting work 
