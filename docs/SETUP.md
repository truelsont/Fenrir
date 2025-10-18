# Setup Guide

Complete guide for building and running Fenrir.

## Quick Start (5 minutes)

```bash
brew install cmake
chmod +x build.sh
./build.sh
cd godot_project && open -a Godot project.godot
# Press F5 to run, Space to test C++ integration
```

## Prerequisites

### macOS
```bash
brew install cmake                  # Build system
brew install --cask godot          # Godot 4.4+
python3 --version                  # Should be 3.8+ (pre-installed)
```

### Ubuntu/Linux
```bash
sudo apt-get install cmake build-essential libboost-all-dev
# Download Godot 4.4+ from godotengine.org
```

**Note**: SCons is auto-installed in `.venv/` by build script.

## Build Options

### Option 1: Automated (Recommended)
```bash
chmod +x build.sh
./build.sh
```

Automatically:
- Creates Python venv + installs SCons
- Clones/updates godot-cpp (4.4 branch)
- Builds godot-cpp bindings
- Builds Fenrir C++ library

### Option 2: Manual Build

#### macOS
```bash
# 1. Setup Python venv
python3 -m venv .venv
source .venv/bin/activate
pip install scons

# 2. Build godot-cpp
cd external/godot-cpp
git checkout 4.4
git submodule update --init
python -m SCons platform=macos target=template_debug
cd ../..

# 3. Build Fenrir
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(sysctl -n hw.ncpu)
cd ..
```

#### Linux
```bash
# Same as macOS, but use:
python -m SCons platform=linux target=template_debug
make -j$(nproc)
```

## Running in Godot

```bash
cd godot_project
open -a Godot project.godot  # macOS
# OR
godot project.godot          # Linux
```

**In Godot**:
1. Press F5 to run
2. Press Space to call C++ code
3. Console should show: "Hello from Fenrir C++ engine!"

## Project Structure

```
fenrir/
├── src/                    # C++ engine code
│   └── godot/              # Godot integration
│       ├── game_node.hh
│       ├── game_node.cc
│       └── register_types.cc
├── godot_project/          # Godot project files
│   ├── project.godot
│   ├── fenrir.gdextension
│   ├── main.gd
│   └── main.tscn
├── bin/                    # Compiled libraries
│   └── libfenrir.dylib
├── external/               # Dependencies
│   ├── godot-cpp/          # Godot C++ bindings
│   └── cmake-4.1.2/        # CMake source (optional)
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

### "Library not loading in Godot"
```bash
# Clean rebuild
rm -rf build/ bin/ external/godot-cpp/bin/
./build.sh
# Restart Godot completely
```

### "GameNode class not found"
- Ensure `godot_project/fenrir.gdextension` exists
- Check it points to `res://../bin/libfenrir.dylib`
- Restart Godot editor
- Check Output tab for GDExtension errors

### "Version mismatch" errors
```bash
# Ensure godot-cpp matches your Godot version
cd external/godot-cpp
git checkout 4.4  # Or 4.3, 4.2, etc.
git submodule update --init
cd ../..
rm -rf build/ bin/
./build.sh
```

### "godot-cpp build failed"
```bash
# Install Xcode tools (macOS)
xcode-select --install

# Or activate venv and retry
source .venv/bin/activate
cd external/godot-cpp
python -m SCons platform=macos target=template_debug
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
3. **Restart Godot** (or Project → Reload Current Project)
4. **Test** changes

**Tip**: Close Godot while rebuilding to avoid file locks.

## Building CMake from Source (Optional)

If you need a custom CMake build (30+ minutes):

```bash
cd external/cmake-4.1.2
./bootstrap --prefix=/usr/local
make -j$(sysctl -n hw.ncpu)
sudo make install
cmake --version
```

**Recommendation**: Use Homebrew/apt instead.

## Verification

After successful build:

```bash
# Library exists
ls -la bin/libfenrir.dylib

# godot-cpp built
ls external/godot-cpp/bin/

# Correct branch
git -C external/godot-cpp branch --show-current  # Should be 4.4
```

## Next Steps

- **Architecture**: See `docs/ARCHITECTURE.md`
- **Roadmap**: See `TODO.md`
- **Development**: Read `llm-system-prompts/` if using AI assistants
