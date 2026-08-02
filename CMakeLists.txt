cmake_minimum_required(VERSION 3.20)

project(
    htpste
    VERSION 0.1.0
    DESCRIPTION "High-Throughput Portfolio Stress Testing Engine"
    LANGUAGES CXX
)

add_library(htpste_core
    src/domain/market_state.cpp
    src/domain/portfolio.cpp
    src/domain/position.cpp
    src/scenarios/deterministic_scenarios.cpp
    src/scenarios/scenario.cpp
)
add_library(htpste::core ALIAS htpste_core)

target_compile_features(htpste_core PUBLIC cxx_std_20)
target_include_directories(htpste_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

option(HTPSTE_BUILD_TESTS "Build the HTPSTE test suite" ON)

if(HTPSTE_BUILD_TESTS)
    include(CTest)
    include(FetchContent)

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(googletest)

    add_subdirectory(tests)
endif()
