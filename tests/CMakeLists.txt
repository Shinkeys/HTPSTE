add_executable(htpste_domain_tests
    market_state_test.cpp
    portfolio_test.cpp
    position_test.cpp
    scenario_test.cpp
)

target_link_libraries(htpste_domain_tests
    PRIVATE
        htpste::core
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(htpste_domain_tests)
