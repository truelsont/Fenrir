#!/bin/bash
set -e

echo "=== Fenrir Build Script ==="
echo ""

# Add common cmake paths to PATH
export PATH="/opt/homebrew/bin:/usr/local/bin:$PATH"

# Parse flags
CLEAN=false
for arg in "$@"; do
    case "$arg" in
        clean|--clean) CLEAN=true ;;
        --help|-h)
            echo "Usage: ./build.sh [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  clean, --clean    Remove build directory before building"
            echo "  --help, -h        Show this help"
            exit 0
            ;;
    esac
done

if [[ "$CLEAN" == true ]]; then
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
elif [[ "$OSTYPE" == "msys" || "$OSTYPE" == "mingw"* || "$OSTYPE" == "cygwin"* ]]; then
    OS="windows"
    NPROC=${NUMBER_OF_PROCESSORS:-4}
else
    echo "ERROR: Unsupported OS: $OSTYPE"
    exit 1
fi

echo "Platform: $OS"
echo ""

# Step 1: Install system dependencies
echo "Checking system dependencies..."

if [[ "$OS" == "macos" ]]; then
    if ! command -v cmake &> /dev/null; then
        echo "Installing cmake via Homebrew..."
        if ! command -v brew &> /dev/null; then
            echo "ERROR: Homebrew not found. Install from https://brew.sh then re-run."
            exit 1
        fi
        brew install cmake
    fi
    echo "✓ CMake found: $(cmake --version | head -n 1)"

elif [[ "$OS" == "linux" ]]; then
    REQUIRED_PKGS="cmake build-essential git python3 libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libgl-dev"
    MISSING_PKGS=""
    for pkg in $REQUIRED_PKGS; do
        if ! dpkg -s "$pkg" &>/dev/null 2>&1; then
            MISSING_PKGS="$MISSING_PKGS $pkg"
        fi
    done
    if [[ -n "$MISSING_PKGS" ]]; then
        echo "Installing missing packages:$MISSING_PKGS"
        sudo apt-get update -qq
        sudo apt-get install -y $MISSING_PKGS
    fi
    echo "✓ All Linux dependencies installed"
    echo "✓ CMake found: $(cmake --version | head -n 1)"

elif [[ "$OS" == "windows" ]]; then
    if ! command -v cmake &> /dev/null; then
        echo "ERROR: CMake not found."
        echo "Install from: https://cmake.org/download/"
        echo "Or with winget: winget install Kitware.CMake"
        exit 1
    fi
    echo "✓ CMake found: $(cmake --version | head -n 1)"
fi

# Step 3: Initialize git submodules
echo ""
echo "Setting up submodules..."

# Core submodules
git submodule update --init external/raylib external/boost external/flecs external/flatbuffers external/eigen

# Dear ImGui (cloned separately, not a submodule)
if [ ! -f "external/imgui/imgui.cpp" ]; then
    echo "Cloning Dear ImGui..."
    git clone --depth 1 --branch v1.91.5 https://github.com/ocornut/imgui.git external/imgui
    echo "✓ Dear ImGui cloned"
else
    echo "✓ Dear ImGui already exists"
fi

# Boost sub-submodules: init core libs then use depinst for transitive deps
echo "Setting up Boost dependencies..."
cd external/boost
git checkout boost-1.85.0 2>/dev/null || true
git submodule update --init tools/boostdep tools/build tools/cmake
git submodule update --init libs/log libs/filesystem libs/system libs/thread libs/config libs/headers
python3 tools/boostdep/depinst/depinst.py log
python3 tools/boostdep/depinst/depinst.py filesystem
python3 tools/boostdep/depinst/depinst.py thread
cd ../..
echo "✓ Boost 1.85.0 initialized"

echo ""

# Step 4: Build Fenrir
echo "Building Fenrir..."
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . -j "$NPROC"
cd ..

# Step 5: Verify output
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
