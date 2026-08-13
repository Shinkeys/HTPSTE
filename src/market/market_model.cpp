#include "htpste/market/market_model.hpp"

#include <Eigen/Cholesky>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <stdexcept>
#include <unordered_set>
#include <utility>

namespace htpste
{
namespace
{
[[nodiscard]] bool isBlank(const std::string& value)
{
    return value.empty() ||
           std::ranges::all_of(value, [](const unsigned char character)
                              { return std::isspace(character) != 0; });
}

void validateSymbols(const std::vector<std::string>& symbols)
{
    if (symbols.empty())
    {
        throw std::invalid_argument(
            "market model must contain at least one symbol");
    }

    std::unordered_set<std::string> unique_symbols;
    unique_symbols.reserve(symbols.size());
    for (const auto& symbol : symbols)
    {
        if (isBlank(symbol))
        {
            throw std::invalid_argument(
                "market model symbols must not be blank");
        }
        if (!unique_symbols.insert(symbol).second)
        {
            throw std::invalid_argument("market model symbols must be unique");
        }
    }
}

void validateDimensions(const std::size_t asset_count,
                        const Eigen::VectorXd& prices,
                        const Eigen::VectorXd& mean_log_returns,
                        const Eigen::MatrixXd& covariance)
{
    const auto expected_size = static_cast<Eigen::Index>(asset_count);
    if (prices.size() != expected_size ||
        mean_log_returns.size() != expected_size ||
        covariance.rows() != expected_size ||
        covariance.cols() != expected_size)
    {
        throw std::invalid_argument(
            "prices, mean log returns, and covariance dimensions must match "
            "the symbol count");
    }
}

void validateValues(const Eigen::VectorXd& prices,
                    const Eigen::VectorXd& mean_log_returns,
                    const Eigen::MatrixXd& covariance)
{
    if (!prices.array().isFinite().all() || (prices.array() <= 0.0).any())
    {
        throw std::invalid_argument(
            "market model prices must be finite and positive");
    }
    if (!mean_log_returns.array().isFinite().all())
    {
        throw std::invalid_argument("mean log returns must be finite");
    }
    if (!covariance.array().isFinite().all())
    {
        throw std::invalid_argument("covariance values must be finite");
    }

    const double covariance_scale =
        std::max(1.0, covariance.cwiseAbs().maxCoeff());
    const double maximum_asymmetry =
        (covariance - covariance.transpose()).cwiseAbs().maxCoeff();
    if (maximum_asymmetry > 1e-12 * covariance_scale)
    {
        throw std::invalid_argument("covariance matrix must be symmetric");
    }

    const Eigen::LLT<Eigen::MatrixXd> decomposition(covariance);
    if (decomposition.info() != Eigen::Success)
    {
        throw std::invalid_argument(
            "covariance matrix must be positive definite");
    }
}

} // namespace

MarketModel::MarketModel(std::vector<std::string> symbols,
                         Eigen::VectorXd prices,
                         Eigen::VectorXd mean_log_returns,
                         Eigen::MatrixXd covariance)
    : m_symbols(std::move(symbols)), m_prices(std::move(prices)),
      m_mean_log_returns(std::move(mean_log_returns)),
      m_covariance(std::move(covariance))
{
    validateSymbols(m_symbols);
    validateDimensions(m_symbols.size(), m_prices, m_mean_log_returns,
                       m_covariance);
    validateValues(m_prices, m_mean_log_returns, m_covariance);
}

std::size_t MarketModel::assetCount() const noexcept
{
    return m_symbols.size();
}

std::span<const std::string> MarketModel::symbols() const noexcept
{
    return m_symbols;
}

const Eigen::VectorXd& MarketModel::prices() const noexcept
{
    return m_prices;
}

const Eigen::VectorXd& MarketModel::meanLogReturns() const noexcept
{
    return m_mean_log_returns;
}

const Eigen::MatrixXd& MarketModel::covariance() const noexcept
{
    return m_covariance;
}

std::optional<std::size_t>
MarketModel::indexOf(const std::string_view symbol) const noexcept
{
    const auto position = std::ranges::find(m_symbols, symbol);
    if (position == m_symbols.end())
    {
        return std::nullopt;
    }
    return static_cast<std::size_t>(
        std::distance(m_symbols.cbegin(), position));
}

} // namespace htpste
