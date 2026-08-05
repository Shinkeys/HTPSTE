#include "htpste/domain/market_state.hpp"
#include "htpste/domain/portfolio.hpp"
#include "htpste/scenarios/deterministic_scenarios.hpp"
#include "htpste/simulation/simulation_engine.hpp"

#include <array>
#include <chrono>
#include <exception>
#include <iomanip>
#include <iostream>
#include <vector>

namespace
{
void printReport(const htpste::SimulationReport& report)
{
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Current portfolio valuation\n";
    for (const auto& position : report.currentPortfolio().positions())
    {
        std::cout << "  " << position.position().symbol() << ": "
                  << position.position().quantity() << " count x "
                  << position.currentPrice() << " = " << position.marketValue()
                  << '\n';
    }
    std::cout << "  Total value: " << report.currentPortfolio().totalValue()
              << '\n';
    std::cout << "  Long exposure: " << report.currentPortfolio().longExposure()
              << '\n';
    std::cout << "  Short exposure: "
              << report.currentPortfolio().shortExposure() << '\n';
    std::cout << "  Gross exposure: "
              << report.currentPortfolio().grossExposure() << "\n\n";

    std::cout << "Scenario repricing\n";
    for (const auto& scenario : report.scenarioResults())
    {
        std::cout << "  [" << scenario.scenarioId() << "] "
                  << scenario.scenarioName()
                  << ": stressed value = " << scenario.stressedPortfolioValue()
                  << ", P&L = " << scenario.pnl() << '\n';
        for (const auto& position : scenario.positions())
        {
            std::cout << "      " << position.symbol() << ": "
                      << position.currentPrice() << " -> "
                      << position.stressedPrice() << " ("
                      << position.relativeShock() * 100.0
                      << "%), P&L = " << position.pnl() << '\n';
        }
    }

    const auto& risk = report.riskMetrics();
    std::cout << "\nRisk summary at " << risk.confidenceLevel() * 100.0
              << "%\n";
    std::cout << "  VaR: " << risk.valueAtRisk() << '\n';
    std::cout << "  CVaR: " << risk.conditionalValueAtRisk() << '\n';
    std::cout << "  Worst scenarios:\n";
    for (const auto& scenario : risk.worstScenarios())
    {
        std::cout << "    [" << scenario.scenarioId() << "] "
                  << scenario.scenarioName() << ": P&L = " << scenario.pnl()
                  << ", loss = " << scenario.loss() << '\n';
    }

    const double elapsed_milliseconds =
        std::chrono::duration<double, std::milli>{report.runtime()}.count();
    std::cout << "\nRuntime: " << elapsed_milliseconds << " ms\n";
    std::cout << "Throughput: " << report.scenariosPerSecond()
              << " scenarios/second\n";
}

} // namespace

int main()
{
    try
    {
        const htpste::Portfolio portfolio{std::vector{
            htpste::Position{"AAPL", 120.0},
            htpste::Position{"MSFT", -40.0},
            htpste::Position{"NVDA", 75.0},
            htpste::Position{"JPM", -60.0},
        }};
        const htpste::MarketState market_state{
            htpste::PriceMap{
                {"AAPL", 210.0},
                {"MSFT", 430.0},
                {"NVDA", 125.0},
                {"JPM", 205.0},
            },
            htpste::ModelParameters{
                htpste::ParameterMap{{"risk_free_rate", 0.03}}}};

        const htpste::CovidCrashScenario covid_crash;
        const htpste::GlobalFinancialCrisisScenario financial_crisis;
        const htpste::DotComCrashScenario dot_com_crash;
        const htpste::RateShockScenario rate_shock;
        const htpste::OilCollapseScenario oil_collapse;
        const htpste::FlashCrashScenario flash_crash;
        const std::array<const htpste::Scenario *, 6> scenarios{
            &covid_crash, &financial_crisis, &dot_com_crash,
            &rate_shock,  &oil_collapse,     &flash_crash,
        };

        const htpste::SimulationEngine engine;
        const htpste::SimulationReport report =
            engine.run(portfolio, market_state, scenarios,
                       htpste::SimulationSettings{0.95, 3});
        printReport(report);
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Simulation failed: " << exception.what() << '\n';
        return 1;
    }
}
