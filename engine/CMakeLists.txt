cmake_minimum_required(VERSION 3.15)
project(OptimalCover)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_executable(OptimalCover
    src/infrastructure/json_parser.cpp
)