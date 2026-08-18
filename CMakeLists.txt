cmake_minimum_required(VERSION 3.20)

project(
    htpste
    VERSION 0.1.0
    DESCRIPTION "High-Throughput Portfolio Stress Testing Engine"
    LANGUAGES CXX
)

# Option to keep clang-based editors in sync with real build flags
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(HTPSTE_BUILD_TESTS "Build the HTPSTE test suite" ON)
option(HTPSTE_ENABLE_SANITIZERS
       "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)

include(FetchContent)
find_package(Threads REQUIRED)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
    GIT_SHALLOW TRUE
)
FetchContent_Declare(
    eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG 3.4.0
    GIT_SHALLOW TRUE
)

# Eigen uses the generic BUILD_TESTING option for its own large test suite.
# Keep dependency tests and documentation out of HTPSTE builds
set(BUILD_TESTING OFF CACHE BOOL "Build dependency test suites" FORCE)
set(EIGEN_BUILD_DOC OFF CACHE BOOL "Build Eigen documentation" FORCE)
FetchContent_MakeAvailable(nlohmann_json eigen)
unset(BUILD_TESTING CACHE)
unset(EIGEN_BUILD_DOC CACHE)

add_library(htpste_core
    src/domain/market_state.cpp
    src/market/market_model.cpp
    src/market/market_model_loader.cpp
    src/domain/portfolio.cpp
    src/domain/position.cpp
    src/pricing/portfolio_valuation.cpp
    src/pricing/scenario_repricing.cpp
    src/risk/risk_metrics.cpp
    src/scenarios/deterministic_scenarios.cpp
    src/scenarios/monte_carlo_scenario.cpp
    src/scenarios/monte_carlo_scenario_generator.cpp
    src/scenarios/scenario.cpp
    src/simulation/simulation_engine.cpp
    src/validation/simulation_validator.cpp
)
add_library(htpste::core ALIAS htpste_core)

if(HTPSTE_ENABLE_SANITIZERS)
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        message(FATAL_ERROR
            "HTPSTE sanitizers require a Clang or GNU compiler")
    endif()

    add_library(htpste_sanitizers INTERFACE)
    target_compile_options(htpste_sanitizers
        INTERFACE
            -fsanitize=address,undefined
            -fno-omit-frame-pointer
            -fno-sanitize-recover=all
    )
    target_link_options(htpste_sanitizers
        INTERFACE -fsanitize=address,undefined
    )
    target_link_libraries(htpste_core PUBLIC htpste_sanitizers)
endif()

target_compile_features(htpste_core PUBLIC cxx_std_20)
target_include_directories(htpste_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(htpste_core
    PUBLIC
        Eigen3::Eigen
    PRIVATE
        Threads::Threads
        nlohmann_json::nlohmann_json
)

add_executable(htpste apps/main.cpp)
target_link_libraries(htpste PRIVATE htpste::core)

if(HTPSTE_BUILD_TESTS)
    include(CTest)

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(googletest)

    add_subdirectory(tests)

    add_test(
        NAME htpste_cli_help
        COMMAND htpste --help
    )
    add_test(
        NAME htpste_cli_simulation_job
        COMMAND htpste
            --market-model
                ${PROJECT_SOURCE_DIR}/tests/fixtures/market_model.json
            --scenarios 16
            --seed 42
            --threads 2
    )
    set_tests_properties(htpste_cli_simulation_job PROPERTIES
        PASS_REGULAR_EXPRESSION "Job 0 complete"
    )
endif()
