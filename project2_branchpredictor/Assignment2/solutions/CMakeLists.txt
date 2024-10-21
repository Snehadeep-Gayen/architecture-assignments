cmake_minimum_required(VERSION 3.12)
project(bpsim)


# export the compile commands for clangd to pick it up
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_BUILD_TYPE Release)

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/branchpredictor.cpp
    src/simulator.cpp
    src/main.cpp
)

# Add executable
add_executable(bpsim ${SOURCES})