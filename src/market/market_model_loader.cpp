#include "htpste/market/market_model_loader.hpp"

#include <nlohmann/json.hpp>

#include <fstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace htpste
{
namespace
{
using Json = nlohmann::json;

[[nodiscard]] const Json& symbolsElement(const Json& model_json)
{
    if (model_json.contains("symbols"))
    {
        return model_json.at("symbols");
    }
    return model_json.at("name");
}

[[nodiscard]] Eigen::VectorXd readVector(const Json& model_json,
                                         const std::string_view field_name)
{
    const Json& values = model_json.at(field_name);
    if (!values.is_array())
    {
        throw std::invalid_argument(std::string{field_name} +
                                    " must be an array");
    }

    Eigen::VectorXd result(static_cast<Eigen::Index>(values.size()));
    for (std::size_t index = 0; index < values.size(); ++index)
    {
        result[static_cast<Eigen::Index>(index)] = values.at(index).get<double>();
    }
    return result;
}

[[nodiscard]] Eigen::MatrixXd readCovariance(const Json& model_json)
{
    const Json& rows = model_json.at("covariance");
    if (!rows.is_array())
    {
        throw std::invalid_argument("covariance must be an array of arrays");
    }

    const auto row_count = static_cast<Eigen::Index>(rows.size());
    Eigen::MatrixXd covariance(row_count, row_count);
    for (Eigen::Index row = 0; row < row_count; ++row)
    {
        const Json& values = rows.at(static_cast<std::size_t>(row));
        if (!values.is_array() ||
            values.size() != static_cast<std::size_t>(row_count))
        {
            throw std::invalid_argument("covariance matrix must be square");
        }
        for (Eigen::Index column = 0; column < row_count; ++column)
        {
            covariance(row, column) =
                values.at(static_cast<std::size_t>(column)).get<double>();
        }
    }
    return covariance;
}

[[nodiscard]] MarketModel parseMarketModel(const Json& model_json)
{
    if (!model_json.is_object())
    {
        throw std::invalid_argument("market model JSON must be an object");
    }

    const std::vector<std::string> symbols =
        symbolsElement(model_json).get<std::vector<std::string>>();
    Eigen::VectorXd prices = readVector(model_json, "prices");
    Eigen::VectorXd mean_log_returns =
        readVector(model_json, "mean_log_returns");
    Eigen::MatrixXd covariance = readCovariance(model_json);
    return MarketModel{symbols, std::move(prices),
                       std::move(mean_log_returns), std::move(covariance)};
}

} // namespace

MarketModel
MarketModelLoader::loadFromFile(const std::filesystem::path& input_path) const
{
    std::ifstream input{input_path};
    if (!input)
    {
        throw std::runtime_error("unable to open market model file: " +
                                 input_path.string());
    }

    try
    {
        return load(input);
    }
    catch (const std::exception& exception)
    {
        throw std::invalid_argument("invalid market model file '" +
                                    input_path.string() + "': " +
                                    exception.what());
    }
}

MarketModel MarketModelLoader::load(std::istream& input) const
{
    try
    {
        Json model_json;
        input >> model_json;
        return parseMarketModel(model_json);
    }
    catch (const nlohmann::json::exception& exception)
    {
        throw std::invalid_argument("invalid market model JSON: " +
                                    std::string{exception.what()});
    }
}

} // namespace htpste
