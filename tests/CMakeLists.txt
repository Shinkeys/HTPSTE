add_executable(htpste_domain_tests
    market_state_test.cpp
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

include(GoogleTest)
gtest_discover_tests(htpste_domain_tests)
