add_executable(htpste_domain_tests
    market_model_loader_test.cpp
    market_state_test.cpp
    monte_carlo_scenario_generator_test.cpp
    portfolio_test.cpp
    portfolio_valuation_test.cpp
    position_test.cpp
    risk_metrics_test.cpp
    scenario_test.cpp
    scenario_repricing_test.cpp
    simulation_engine_test.cpp
    simulation_validator_test.cpp
)

target_link_libraries(htpste_domain_tests
    PRIVATE
        htpste::core
        GTest::gtest_main
)

target_compile_definitions(htpste_domain_tests
    PRIVATE HTPSTE_SOURCE_DIR="${PROJECT_SOURCE_DIR}"
)

include(GoogleTest)
gtest_discover_tests(htpste_domain_tests)
