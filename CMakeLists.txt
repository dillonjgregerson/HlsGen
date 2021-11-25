cmake_minimum_required(VERSION 3.13)  # CMake version check
project(dpgen)               # Create project "dpgen"
set(CMAKE_CXX_STANDARD 14)            # Enable c++14 standard

# Add main.cpp file of project root directory as source file
set(SOURCE_FILES src/HlsGen.cpp src/Vertex.cpp src/BaseType.cpp src/dpgen.cpp)

# Add executable target with source files listed in SOURCE_FILES variable
add_executable(dpgen ${SOURCE_FILES})