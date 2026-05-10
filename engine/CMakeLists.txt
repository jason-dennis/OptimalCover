cmake_minimum_required(VERSION 3.15)
project(OptimalCover)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ── Google Test ────────────────────────────────────────────────
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# ── Librarie cu logica proiectului (fara main) ─────────────────
add_library(OptimalCoverLib
    src/infrastructure/json_parser.cpp
    src/solvers/greedy_solver.cpp
    src/domain/WIFIobjective.cpp
)
target_include_directories(OptimalCoverLib PUBLIC include)

# ── Executabilul principal ─────────────────────────────────────
add_executable(OptimalCover
    src/main.cpp
)
target_link_libraries(OptimalCover PRIVATE OptimalCoverLib)

# ── Executabilul de teste ──────────────────────────────────────
enable_testing()

add_executable(OptimalCoverTests
    tests/greedy_solver_test.cpp
)
target_include_directories(OptimalCoverTests PRIVATE include)
target_link_libraries(OptimalCoverTests PRIVATE
    OptimalCoverLib
    GTest::gtest
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(OptimalCoverTests)