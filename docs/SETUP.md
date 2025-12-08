# Setup Guide

Complete guide for building and running Fenrir.

## Quick Start (5 minutes)

```bash
brew install cmake  # macOS only
chmod +x build.sh
./build.sh
./bin/fenrir
# Use arrow keys to pan, mouse wheel to zoom, Space for debug
```

## Prerequisites

### macOS
```bash
brew install cmake                  # Build system
python3 --version                  # Should be 3.8+ (pre-installed)
```

### Ubuntu/Linux
```bash
sudo apt-get install cmake build-essential
sudo apt-get install libgl1-mesa-dev libx11-dev  # For Raylib
```

**Note**: 
- Boost 1.85.0 is vendored as a git submodule (no system install needed)
- Raylib 5.0+ is vendored as a git submodule
- Dear ImGui 1.90.0 is cloned during build

## Build Options

### Option 1: Automated (Recommended)
```bash
chmod +x build.sh
./build.sh
```

Automatically:
- Initializes Boost 1.85.0 submodule (if needed)
- Initializes Raylib submodule (if needed)
- Clones Dear ImGui (if needed)
- Builds Raylib
- Builds Fenrir executable

### Option 2: Manual Build

#### macOS / Linux
```bash
# 1. Initialize submodules
git submodule update --init external/boost external/raylib
cd external/boost && git checkout boost-1.85.0 && cd ../..

# 2. Setup Boost dependencies
cd external/boost
git submodule update --init tools/boostdep tools/build
for lib in log filesystem system thread; do
    python3 tools/boostdep/depinst/depinst.py --include example $lib
done
cd ../..

# 3. Clone Dear ImGui
git clone --depth 1 --branch v1.90.0 https://github.com/ocornut/imgui.git external/imgui
curl -L https://raw.githubusercontent.com/raylib-extras/rlImGui/main/rlImGui.h -o external/imgui/rlImGui.h
curl -L https://raw.githubusercontent.com/raylib-extras/rlImGui/main/rlImGui.cpp -o external/imgui/rlImGui.cpp

# 4. Build Fenrir
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)  # or $(sysctl -n hw.ncpu) on macOS
cd ..
```

## Running Fenrir

```bash
./bin/fenrir
```

**Controls**:
- Arrow keys: Pan camera
- Mouse wheel: Zoom in/out
- Space: Trigger debug output
- ESC: Exit

## Project Structure

```
fenrir/
├── src/                    # C++ engine code
│   ├── main.cc             # Application entry point
│   ├── ui/                 # Rendering and UI
│   │   ├── renderer.hh/cc
│   │   ├── input_handler.hh/cc
│   │   └── ui_overlay.hh/cc
│   ├── state/              # World state
│   ├── engine/             # Simulation engines
│   ├── compute/            # Compute facade
│   └── util/               # Utilities
├── bin/                    # Compiled executable
│   └── fenrir
├── external/               # Dependencies
│   ├── raylib/             # Raylib 5.0+ (submodule)
│   ├── imgui/              # Dear ImGui 1.90.0 (cloned)
│   └── boost/              # Boost 1.85.0 LTS (submodule)
├── docs/                   # Documentation
├── build.sh                # Automated build script
└── CMakeLists.txt          # Build configuration
```

## Troubleshooting

### "CMake not found"
```bash
brew install cmake  # macOS
sudo apt-get install cmake  # Linux
```

### "Boost not found"
```bash
# Boost is vendored, ensure submodule is initialized
git submodule update --init external/boost
cd external/boost && git checkout boost-1.85.0
```

### "Executable crashes on startup"
```bash
# Clean rebuild
rm -rf build/ bin/
./build.sh
```

### "Raylib not found"
```bash
# Ensure submodule is initialized
git submodule update --init external/raylib
```

### "ImGui errors"
```bash
# Re-download ImGui
rm -rf external/imgui
git clone --depth 1 --branch v1.90.0 https://github.com/ocornut/imgui.git external/imgui
curl -L https://raw.githubusercontent.com/raylib-extras/rlImGui/main/rlImGui.h -o external/imgui/rlImGui.h
curl -L https://raw.githubusercontent.com/raylib-extras/rlImGui/main/rlImGui.cpp -o external/imgui/rlImGui.cpp
```

## Clean Build

```bash
# Remove all build artifacts
rm -rf build/ bin/ lib/ external/godot-cpp/bin/ .venv/

# Rebuild from scratch
./build.sh
```

## Development Workflow

1. **Edit C++ code** in `src/`
2. **Rebuild**: `cd build && make`
3. **Run**: `./bin/fenrir`
4. **Test** changes

**Tip**: Use `make -j$(nproc)` for faster parallel builds.


## Verification

After successful build:

```bash
# Executable exists
ls -la bin/fenrir

# Raylib built
ls external/raylib/src/libraylib.a

# ImGui exists
ls external/imgui/*.cpp
```

## Next Steps

- **Architecture**: See `docs/ARCHITECTURE.md`
- **Roadmap**: See `TODO.md`
- **Development**: Read `llm-system-prompts/` if using AI assistants
