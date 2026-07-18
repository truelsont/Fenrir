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

# Boost (vendored in external/boost, built via add_subdirectory)
if(EXISTS ${CMAKE_SOURCE_DIR}/external/boost/CMakeLists.txt)
    message(STATUS "Building vendored Boost 1.85")
    set(BOOST_INCLUDE_LIBRARIES log filesystem system thread CACHE STRING "" FORCE)
    set(BOOST_ENABLE_CMAKE ON CACHE BOOL "" FORCE)
    set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
    add_subdirectory(external/boost EXCLUDE_FROM_ALL)
    set(BOOST_LIBS Boost::log Boost::filesystem)
else()
    message(FATAL_ERROR "Boost not found! Run: git submodule update --init external/boost")
endif()

# Add Eigen (header-only)
if(EXISTS ${CMAKE_SOURCE_DIR}/external/eigen/CMakeLists.txt)
    message(STATUS "Found Eigen")
    set(EIGEN_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/external/eigen)
else()
    message(FATAL_ERROR "Eigen not found! Run: git submodule add https://gitlab.com/libeigen/eigen.git external/eigen")
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
        src/world/globe_generator.cc
        src/engine/simulation.cc
        src/engine/systems.cc
    )
    
    target_include_directories(fenrir PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src
        ${EIGEN_INCLUDE_DIR}
    )
    
    target_link_libraries(fenrir PRIVATE
        raylib
        flecs::flecs_static
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
    # Globe viewer executable
    add_executable(globe_viewer
        src/globe_viewer.cc
        src/world/globe_generator.cc
    )

    target_include_directories(globe_viewer PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src
        ${EIGEN_INCLUDE_DIR}
    )

    target_link_libraries(globe_viewer PRIVATE
        raylib
        flecs::flecs_static
    )

    if(MACOS)
        target_link_libraries(globe_viewer PRIVATE
            "-framework IOKit"
            "-framework Cocoa"
            "-framework OpenGL"
        )
    elseif(LINUX)
        target_link_libraries(globe_viewer PRIVATE GL m pthread dl rt X11)
    endif()

else()
    message(FATAL_ERROR "Raylib not found! Run: git submodule add https://github.com/raysan5/raylib external/raylib")
endif()

# Tests
add_executable(fenrir_tests
    tests/test_main.cc
    src/world/globe_generator.cc
    src/engine/simulation.cc
    src/engine/systems.cc
)
target_include_directories(fenrir_tests PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${EIGEN_INCLUDE_DIR}
)
target_link_libraries(fenrir_tests PRIVATE
    flecs::flecs_static
)
