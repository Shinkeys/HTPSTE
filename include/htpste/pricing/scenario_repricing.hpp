#ifndef SCENARIO_REPRICING_H
#define SCENARIO_REPRICING_H

#include "htpste/pricing/portfolio_valuation.hpp"
#include "htpste/scenarios/scenario.hpp"

#include <cstdint>
#include <span>
#include <string>
#include <vector>

namespace htpste
{
class PositionRepricing final
{
  public:
    PositionRepricing(std::string symbol, double quantity, double current_price,
                      double relative_shock, double stressed_price,
                      double current_value, double stressed_value, double pnl);

    [[nodiscard]] const std::string& symbol() const noexcept;
    [[nodiscard]] double quantity() const noexcept;
    [[nodiscard]] double currentPrice() const noexcept;
    [[nodiscard]] double relativeShock() const noexcept;
    [[nodiscard]] double stressedPrice() const noexcept;
    [[nodiscard]] double currentValue() const noexcept;
    [[nodiscard]] double stressedValue() const noexcept;
    [[nodiscard]] double pnl() const noexcept;

  private:
    std::string m_symbol;
    double m_quantity;
    double m_current_price;
    double m_relative_shock;
    double m_stressed_price;
    double m_current_value;
    double m_stressed_value;
    double m_pnl;
};

class ScenarioResult final
{
  public:
    ScenarioResult(std::uint64_t scenario_id, std::string scenario_name,
                   double current_portfolio_value,
                   std::vector<PositionRepricing> positions);

    [[nodiscard]] std::uint64_t scenarioId() const noexcept;
    [[nodiscard]] const std::string& scenarioName() const noexcept;
    [[nodiscard]] double currentPortfolioValue() const noexcept;
    [[nodiscard]] double stressedPortfolioValue() const noexcept;
    [[nodiscard]] double pnl() const noexcept;
    [[nodiscard]] double loss() const noexcept;
    [[nodiscard]] double absoluteLoss() const noexcept;
    [[nodiscard]] std::span<const PositionRepricing> positions() const noexcept;

  private:
    std::uint64_t m_scenario_id;
    std::string m_scenario_name;
    double m_current_portfolio_value;
    double m_stressed_portfolio_value;
    double m_pnl;
    std::vector<PositionRepricing> m_positions;
};

class ScenarioRepricer final
{
  public:
    [[nodiscard]] ScenarioResult
    reprice(const PortfolioValuation& current_portfolio,
            const Scenario& scenario, std::uint64_t scenario_id) const;
};

} // namespace htpste

#endif // SCENARIO_REPRICING_H
