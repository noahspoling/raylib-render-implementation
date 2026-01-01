cmake_minimum_required(VERSION 3.27)
project(gramarye-renderer-raylib C)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# WebAssembly toggle
option(BUILD_WEB "Build with Emscripten (WebAssembly)" OFF)

if(EMSCRIPTEN OR BUILD_WEB)
    set(PLATFORM Web CACHE STRING "" FORCE)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

# Option to build as shared or static (default static)
option(BUILD_SHARED_LIBS "Build shared library" OFF)

# Fetch gramarye-renderer-interface (or use local if USE_LOCAL_MODULES is set)
include(FetchContent)
# Check if target already exists (from parent using local modules)
if(NOT TARGET gramarye-renderer-interface)
    if(USE_LOCAL_MODULES AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../gramarye-renderer-interface/CMakeLists.txt")
        message(STATUS "gramarye-raylib-implementation: Using local gramarye-renderer-interface")
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../gramarye-renderer-interface gramarye-renderer-interface)
    else()
        FetchContent_Declare(
            gramarye_renderer_interface
            GIT_REPOSITORY "https://github.com/noahspoling/gramarye-renderer-interface.git"
            GIT_TAG "master"
            GIT_PROGRESS TRUE
            GIT_SHALLOW TRUE
        )
        FetchContent_MakeAvailable(gramarye_renderer_interface)
    endif()
endif()

# Raylib is expected to be provided by the parent project
# We'll link against it if available, but don't fetch it here

# Collect source files
file(GLOB SRC_FILES "src/*.c")

# Create library
add_library(gramarye-renderer-raylib 
    ${SRC_FILES}
)

# Add include directories for clay (if available)
# Clay headers should be provided by the parent project

# Add include directories
target_include_directories(gramarye-renderer-raylib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Link against dependencies
target_link_libraries(gramarye-renderer-raylib PUBLIC 
    gramarye-renderer-interface
)

# Raylib should be linked by parent project, but we need its headers
# The parent project should set raylib_SOURCE_DIR before including this

# Export for CMake
option(GRAMARYE_RENDERER_RAYLIB_INSTALL "Install gramarye-renderer-raylib" OFF)

if(GRAMARYE_RENDERER_RAYLIB_INSTALL)
    include(GNUInstallDirs)
    install(TARGETS gramarye-renderer-raylib
        EXPORT rendererRaylibTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    install(EXPORT rendererRaylibTargets
        FILE rendererRaylibTargets.cmake
        NAMESPACE gramarye-renderer-raylib::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/renderer-raylib
    )
endif()

# For FetchContent compatibility
if(NOT TARGET gramarye-renderer-raylib::gramarye-renderer-raylib)
    add_library(gramarye-renderer-raylib::gramarye-renderer-raylib ALIAS gramarye-renderer-raylib)
endif()

