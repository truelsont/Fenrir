#!/bin/bash
set -e

echo "=== Fenrir Build Script ==="
echo ""

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

# Step 2: Check for Boost
echo "Checking for Boost..."
if [[ "$OS" == "macos" ]]; then
    if brew list boost &> /dev/null 2>&1; then
        echo "✓ Boost found: $(brew list --versions boost)"
    else
        echo "Boost not found. Installing..."
        brew install boost
        echo "✓ Boost installed"
    fi
else
    # Linux - check if boost is installed
    if ldconfig -p | grep -q libboost_log; then
        echo "✓ Boost found"
    else
        echo "ERROR: Boost not found!"
        echo "Install with: sudo apt-get install libboost-all-dev"
        exit 1
    fi
fi

echo ""

# Step 3: Setup Python virtual environment
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

# Step 4: Setup godot-cpp if needed
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

# Step 5: Build godot-cpp
if [[ "$OS" == "macos" ]]; then
    GODOT_CPP_LIB="external/godot-cpp/bin/libgodot-cpp.macos.template_debug.universal.a"
    SCONS_PLATFORM="macos"
else
    GODOT_CPP_LIB="external/godot-cpp/bin/libgodot-cpp.linux.template_debug.x86_64.a"
    SCONS_PLATFORM="linux"
fi

if [ ! -f "$GODOT_CPP_LIB" ]; then
    echo ""
    echo "Building godot-cpp for $OS (this takes a few minutes)..."
    cd external/godot-cpp
    python -m SCons platform=$SCONS_PLATFORM target=template_debug
    cd ../..
    echo "✓ godot-cpp built"
else
    echo "✓ godot-cpp already built"
fi

# Step 6: Build Fenrir
echo ""
echo "Building Fenrir..."
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$NPROC
cd ..

# Step 7: Verify output
if [[ "$OS" == "macos" ]]; then
    LIB_FILE="bin/libfenrir.dylib"
else
    LIB_FILE="bin/libfenrir.so"
fi

if [ -f "$LIB_FILE" ]; then
    echo ""
    echo "=== BUILD SUCCESS ==="
    echo "✓ Library: $LIB_FILE"
    echo "✓ Virtual environment: .venv/"
    echo "✓ Platform: $OS"
    echo ""
    echo "Next steps:"
    echo "1. cd godot_project"
    if [[ "$OS" == "macos" ]]; then
        echo "2. open -a Godot project.godot"
    else
        echo "2. godot project.godot"
    fi
    echo "3. Press F5 to run"
else
    echo ""
    echo "=== BUILD FAILED ==="
    echo "Library not found at $LIB_FILE"
    exit 1
fi
