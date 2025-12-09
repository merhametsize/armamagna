cmake_minimum_required(VERSION 3.26)
project(ArmaMagna LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(include)
include_directories(include/ankerl)

# Source files
file(GLOB SOURCES "src/*.cpp")

# Executable
add_executable(armamagna ${SOURCES})

# Explicit flags per build type
# Release: optimized, no debug info, disable asserts
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -Wall -Wextra -Wconversion -pedantic")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "")

# Debug: no optimization, debug info, sanitizers, full warnings, asserts enabled
set(CMAKE_CXX_FLAGS_DEBUG "-O2 -g -fsanitize=address,undefined,leak -fno-omit-frame-pointer -Wall -Wextra -Wconversion -pedantic")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "-fsanitize=address,undefined,leak")