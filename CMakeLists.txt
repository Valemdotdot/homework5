
cmake_minimum_required(VERSION 3.11)

project(toy-simulator)

set(CMAKE_CXX_STANDARD 17)

add_executable(toy-simulator
    src/main.cpp
    src/cpu.cpp
    src/memory.cpp
    src/decoder.cpp
)