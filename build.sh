#!/bin/bash
set -e

echo "=== Fenrir MVP Build Script ==="
echo ""

# Step 1: Check for CMake
if ! command -v cmake &> /dev/null; then
    echo "ERROR: CMake not found!"
    echo "Install with: brew install cmake"
    exit 1
fi

echo "✓ CMake found: $(cmake --version | head -n 1)"
echo ""

# Step 2: Setup Python virtual environment
if [ ! -d ".venv" ]; then
    echo "Creating Python virtual environment..."
    python3 -m venv .venv
    echo "✓ Virtual environment created"
else
    echo "✓ Virtual environment already exists"
fi

# Activate virtual environment
echo "Activating virtual environment..."
source .venv/bin/activate

# Install SCons if not present
if ! python -m pip show scons &> /dev/null; then
    echo "Installing SCons..."
    pip install scons
    echo "✓ SCons installed"
else
    echo "✓ SCons already installed"
fi

echo ""

# Step 3: Setup godot-cpp if needed
if [ ! -d "external/godot-cpp" ]; then
    echo "Setting up godot-cpp..."
    git submodule add https://github.com/godotengine/godot-cpp external/godot-cpp
    cd external/godot-cpp
    git checkout 4.4
    git submodule update --init
    cd ../..
    echo "✓ godot-cpp cloned"
else
    echo "✓ godot-cpp already exists"
    # Ensure we're on the right branch
    cd external/godot-cpp
    CURRENT_BRANCH=$(git branch --show-current)
    if [ "$CURRENT_BRANCH" != "4.4" ]; then
        echo "Switching godot-cpp to 4.4 branch..."
        git checkout 4.4
        git submodule update --init
    fi
    cd ../..
fi

# Step 4: Build godot-cpp
if [ ! -f "external/godot-cpp/bin/libgodot-cpp.macos.template_debug.universal.a" ]; then
    echo ""
    echo "Building godot-cpp (this takes a few minutes)..."
    cd external/godot-cpp
    python -m SCons platform=macos target=template_debug
    cd ../..
    echo "✓ godot-cpp built"
else
    echo "✓ godot-cpp already built"
fi

# Step 5: Build Fenrir
echo ""
echo "Building Fenrir..."
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(sysctl -n hw.ncpu)
cd ..

# Step 6: Verify output
if [ -f "bin/libfenrir.dylib" ]; then
    echo ""
    echo "=== BUILD SUCCESS ==="
    echo "✓ Library: bin/libfenrir.dylib"
    echo "✓ Virtual environment: .venv/"
    echo ""
    echo "Next steps:"
    echo "1. Open project.godot in Godot 4.2+"
    echo "2. Run main.tscn"
    echo "3. Press Space to call C++ code"
    echo ""
    echo "Note: Virtual environment is active in this shell."
    echo "To deactivate: deactivate"
else
    echo ""
    echo "=== BUILD FAILED ==="
    echo "Library not found at bin/libfenrir.dylib"
    exit 1
fi
