#ifndef HTPSTE_MARKET_MODEL_HPP
#define HTPSTE_MARKET_MODEL_HPP

#include <Eigen/Core>

#include <cstddef>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace htpste
{
class MarketModel final
{
  public:
    MarketModel(std::vector<std::string> symbols, Eigen::VectorXd prices,
                Eigen::VectorXd mean_log_returns,
                Eigen::MatrixXd covariance);

    [[nodiscard]] std::size_t assetCount() const noexcept;
    [[nodiscard]] std::span<const std::string> symbols() const noexcept;
    [[nodiscard]] const Eigen::VectorXd& prices() const noexcept;
    [[nodiscard]] const Eigen::VectorXd& meanLogReturns() const noexcept;
    [[nodiscard]] const Eigen::MatrixXd& covariance() const noexcept;
    [[nodiscard]] std::optional<std::size_t>
    indexOf(std::string_view symbol) const noexcept;

  private:
    std::vector<std::string> m_symbols;
    Eigen::VectorXd m_prices;
    Eigen::VectorXd m_mean_log_returns;
    Eigen::MatrixXd m_covariance;
};

} // namespace htpste

#endif // HTPSTE_MARKET_MODEL_HPP
