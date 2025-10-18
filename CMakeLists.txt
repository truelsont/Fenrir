cmake_minimum_required(VERSION 3.20)
project(Fenrir VERSION 0.1.0 LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform detection
if(APPLE)
    message(STATUS "Platform: macOS")
    set(MACOS TRUE)
endif()

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Output directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Find Boost (optional for now, will add later)
# find_package(Boost 1.70 COMPONENTS log system)

# Add godot-cpp if it exists
if(EXISTS ${CMAKE_SOURCE_DIR}/external/godot-cpp/CMakeLists.txt)
    message(STATUS "Found godot-cpp, building GDExtension")
    add_subdirectory(external/godot-cpp)
    
    # Create minimal GDExtension library
    add_library(fenrir SHARED
        src/godot/register_types.cc
        src/godot/game_node.cc
    )
    
    target_include_directories(fenrir PRIVATE
        ${CMAKE_SOURCE_DIR}
    )
    
    target_link_libraries(fenrir PRIVATE
        godot-cpp
    )
    
    # macOS specific settings
    if(MACOS)
        set_target_properties(fenrir PROPERTIES
            OUTPUT_NAME "libfenrir"
            PREFIX ""
            SUFFIX ".dylib"
        )
    endif()
    
    message(STATUS "Output: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libfenrir.dylib")
else()
    message(FATAL_ERROR "godot-cpp not found! Run: git submodule add https://github.com/godotengine/godot-cpp external/godot-cpp")
endif()
