cmake_minimum_required(VERSION 3.12)
project(cachesim)


# export the compile commands for clangd to pick it up
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/GenCache.cpp
    src/FullAssocCache.cpp
    src/Main.cpp
    src/Simulator.cpp
    src/Cache.cpp
)

# Add executable
add_executable(cachesim ${SOURCES})