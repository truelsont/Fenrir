#!/bin/bash
set -e

echo "=== Fenrir Build Script ==="
echo ""

# Add common cmake paths to PATH
export PATH="/opt/homebrew/bin:/usr/local/bin:$PATH"

# Check for clean flag
if [[ "$1" == "clean" ]]; then
    echo "Cleaning build directory..."
    rm -rf build
    echo "✓ Build directory cleaned"
    echo ""
fi

# Detect OS
if [[ "$OSTYPE" == "darwin"* ]]; then
    OS="macos"
    NPROC=$(sysctl -n hw.ncpu)
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    OS="linux"
    NPROC=$(nproc)
else
    echo "ERROR: Unsupported OS: $OSTYPE"
    exit 1
fi

echo "Platform: $OS"
echo ""

# Step 1: Check for CMake
if ! command -v cmake &> /dev/null; then
    echo "ERROR: CMake not found!"
    if [[ "$OS" == "macos" ]]; then
        echo "Install with: brew install cmake"
    else
        echo "Install with: sudo apt-get install cmake"
    fi
    exit 1
fi

echo "✓ CMake found: $(cmake --version | head -n 1)"

# Step 2: Setup Boost submodule
echo "Setting up Boost..."
if [ ! -d "external/boost/.git" ]; then
    echo "Initializing Boost submodule..."
    git submodule update --init external/boost
    cd external/boost
    git checkout boost-1.85.0
    echo "Installing Boost dependencies..."
    git submodule update --init tools/boostdep tools/build
    for lib in log filesystem system thread; do
        python3 tools/boostdep/depinst/depinst.py --include example $lib
    done
    cd ../..
    echo "✓ Boost 1.85.0 initialized"
else
    echo "✓ Boost submodule already initialized"
fi

echo ""

# Step 3: Setup Raylib submodule
echo "Setting up Raylib..."
if [ ! -d "external/raylib/.git" ]; then
    echo "Initializing Raylib submodule..."
    git submodule update --init external/raylib
    echo "✓ Raylib initialized"
else
    echo "✓ Raylib submodule already initialized"
fi

echo ""

# Step 4: Setup Dear ImGui
echo "Setting up Dear ImGui..."
if [ ! -d "external/imgui" ]; then
    echo "Cloning Dear ImGui..."
    git clone --depth 1 --branch v1.91.5 https://github.com/ocornut/imgui.git external/imgui
    echo "✓ Dear ImGui cloned"
else
    echo "✓ Dear ImGui already exists"
fi

echo ""

# Step 5: Build Fenrir
echo ""
echo "Building Fenrir..."
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$NPROC
cd ..

# Step 6: Verify output
EXE_FILE="bin/fenrir"

if [ -f "$EXE_FILE" ]; then
    echo ""
    echo "=== BUILD SUCCESS ==="
    echo "✓ Executable: $EXE_FILE"
    echo "✓ Platform: $OS"
    echo ""
    echo "Next steps:"
    echo "1. ./bin/fenrir"
    echo "2. Use arrow keys to pan, mouse wheel to zoom"
    echo "3. Press Space for debug output"
else
    echo ""
    echo "=== BUILD FAILED ==="
    echo "Executable not found at $EXE_FILE"
    exit 1
fi
