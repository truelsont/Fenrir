cmake_minimum_required(VERSION 3.20)
project(Fenrir VERSION 0.1.0 LANGUAGES CXX)

# Export compile commands for C++ linters/IDEs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Find Boost (vendored in external/boost)
set(BOOST_ROOT ${CMAKE_SOURCE_DIR}/external/boost)
set(Boost_NO_SYSTEM_PATHS ON)
set(Boost_NO_WARN_NEW_VERSIONS ON)

# Try new-style config first (Boost 1.70+)
find_package(Boost 1.85 CONFIG QUIET COMPONENTS log filesystem)
if(NOT Boost_FOUND)
    # Fall back to old FindBoost module
    find_package(Boost 1.85 REQUIRED COMPONENTS log system filesystem thread)
endif()

if(Boost_FOUND)
    message(STATUS "✓ Boost version: ${Boost_VERSION} (vendored)")
    if(TARGET Boost::log)
        message(STATUS "✓ Using Boost::log target (new style)")
        set(BOOST_LIBS Boost::log Boost::filesystem)
    else()
        message(STATUS "✓ Using Boost_LIBRARIES (old style)")
        set(BOOST_LIBS ${Boost_LIBRARIES})
    endif()
endif()

# Add Flecs ECS
if(EXISTS ${CMAKE_SOURCE_DIR}/external/flecs/CMakeLists.txt)
    message(STATUS "Found Flecs ECS")
    set(FLECS_STATIC ON CACHE BOOL "" FORCE)
    add_subdirectory(external/flecs)
else()
    message(FATAL_ERROR "Flecs not found! Run: git submodule add https://github.com/SanderMertens/flecs.git external/flecs")
endif()

# Add FlatBuffers
if(EXISTS ${CMAKE_SOURCE_DIR}/external/flatbuffers/CMakeLists.txt)
    message(STATUS "Found FlatBuffers")
    set(FLATBUFFERS_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(FLATBUFFERS_BUILD_FLATC ON CACHE BOOL "" FORCE)
    add_subdirectory(external/flatbuffers)
    set(FLATBUFFERS_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/external/flatbuffers/include)
else()
    message(WARNING "FlatBuffers not found - serialization features disabled")
endif()

# Add Raylib if it exists
if(EXISTS ${CMAKE_SOURCE_DIR}/external/raylib/CMakeLists.txt)
    message(STATUS "Found Raylib, building application")

    # Raylib build options
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
    add_subdirectory(external/raylib)
    
    # Dear ImGui sources
    set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/external/imgui)
    set(IMGUI_SOURCES
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
    )
    
    # Create executable
    add_executable(fenrir
        src/main.cc
        src/ui/renderer.cc
        src/ui/input_handler.cc
        src/ui/ui_overlay.cc
        src/ui/mapMode/map_mode_registry.cc
        src/ui/mapMode/map_mode_selector.cc
        src/ui/mapMode/strategies/political_mode.cc
        src/ui/mapMode/strategies/province_debug_mode.cc
        src/state/worldState/world_manager.cc
        ${IMGUI_SOURCES}
    )
    
    target_include_directories(fenrir PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/util
        ${CMAKE_SOURCE_DIR}/src/util/generics
        ${CMAKE_SOURCE_DIR}/src/state/worldState
        ${CMAKE_SOURCE_DIR}/src/ui
        ${IMGUI_DIR}
        ${Boost_INCLUDE_DIRS}
        ${FLATBUFFERS_INCLUDE_DIR}
    )
    
    target_link_libraries(fenrir PRIVATE
        raylib
        flecs::flecs_static
        ${BOOST_LIBS}
    )
    
    # Platform-specific settings
    if(MACOS)
        target_link_libraries(fenrir PRIVATE
            "-framework IOKit"
            "-framework Cocoa"
            "-framework OpenGL"
        )
        message(STATUS "Output: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/fenrir")
    elseif(LINUX)
        target_link_libraries(fenrir PRIVATE
            GL
            m
            pthread
            dl
            rt
            X11
        )
        message(STATUS "Output: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/fenrir")
    endif()
else()
    message(FATAL_ERROR "Raylib not found! Run: git submodule add https://github.com/raysan5/raylib external/raylib")    
endif()


