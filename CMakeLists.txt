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
elseif(UNIX AND NOT APPLE)
    message(STATUS "Platform: Linux")
    set(LINUX TRUE)
endif()

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Output directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Find Boost (required)
# Boost 1.89+ uses new CMake config, older versions use FindBoost module
set(Boost_NO_WARN_NEW_VERSIONS ON)

# Try new-style config first (Boost 1.89+)
find_package(Boost 1.70 CONFIG QUIET COMPONENTS log filesystem)
if(NOT Boost_FOUND)
    # Fall back to old FindBoost module
    find_package(Boost 1.70 REQUIRED COMPONENTS log system filesystem thread)
endif()

if(Boost_FOUND)
    message(STATUS "✓ Boost version: ${Boost_VERSION}")
    if(TARGET Boost::log)
        message(STATUS "✓ Using Boost::log target (new style)")
        set(BOOST_LIBS Boost::log Boost::filesystem)
    else()
        message(STATUS "✓ Using Boost_LIBRARIES (old style)")
        set(BOOST_LIBS ${Boost_LIBRARIES})
    endif()
endif()

# Add godot-cpp if it exists
if(EXISTS ${CMAKE_SOURCE_DIR}/external/godot-cpp/CMakeLists.txt)
    message(STATUS "Found godot-cpp, building GDExtension")
    add_subdirectory(external/godot-cpp)

    
    # Create minimal GDExtension library
    add_library(fenrir SHARED
        src/godot/register_types.cc
        src/godot/game_node.cc
        src/state/worldState/worldManager.cc
    )
    
    target_include_directories(fenrir PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/util
        ${CMAKE_SOURCE_DIR}/src/util/generics
        ${CMAKE_SOURCE_DIR}/src/state/worldState
        ${Boost_INCLUDE_DIRS}
    )
    
    target_link_libraries(fenrir PRIVATE
        godot-cpp
        ${BOOST_LIBS}
    )
    
    # Platform-specific settings
    if(MACOS)
        set_target_properties(fenrir PROPERTIES
            OUTPUT_NAME "libfenrir"
            PREFIX ""
            SUFFIX ".dylib"
        )
        message(STATUS "Output: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libfenrir.dylib")
    elseif(LINUX)
        set_target_properties(fenrir PROPERTIES
            OUTPUT_NAME "libfenrir"
            PREFIX ""
            SUFFIX ".so"
        )
        message(STATUS "Output: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libfenrir.so")
    endif()
else()
    message(FATAL_ERROR "godot-cpp not found! Run: git submodule add https://github.com/godotengine/godot-cpp external/godot-cpp")    
endif()


