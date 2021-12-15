cmake_minimum_required(VERSION 3.13)  # CMake version check
project(hlsyn)               # Create project "dpgen"
set(CMAKE_CXX_STANDARD 14)            # Enable c++14 standard

# Add main.cpp file of project root directory as source file
set(SOURCE_FILES src/HlsGen.cpp src/Vertex.cpp src/BaseType.cpp src/hlsyn.cpp)

file(MAKE_DIRECTORY ./src)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ./src)

add_executable(hlsyn ${SOURCE_FILES})
