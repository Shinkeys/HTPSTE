import argparse
import json
from pathlib import Path
from typing import Literal

import numpy as np
import pandas as pd
import yfinance as yf


YAHOO_TICKERS = (
    "AAPL",
    "MSFT",
    "NVDA",
    "AMZN",
    "GOOGL",
    "META",
    "AVGO",
    "TSLA",
    "JPM",
    "LLY",
    "MU",
    "BRK-B",
    "XOM",
    "JNJ",
    "WMT",
)

MODEL_SYMBOLS = tuple(
    "BRK.B" if ticker == "BRK-B" else ticker for ticker in YAHOO_TICKERS
)


def parseArguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Build a calibrated one-day equity market model."
    )
    parser.add_argument(
        "--start",
        default="2022-01-01",
        help="First historical date to request (inclusive).",
    )
    parser.add_argument(
        "--end",
        default=None,
        help="Last historical date to request (exclusive). Defaults to today.",
    )
    parser.add_argument(
        "--output",
        type=Path,
        default=Path(__file__).with_name("market_model.json"),
        help="Destination JSON file.",
    )
    return parser.parse_args()


def downloadClosePrices(start: str, end: str | None) -> pd.DataFrame:
    data = yf.download(
        list(YAHOO_TICKERS),
        start=start,
        end=end,
        auto_adjust=True,
        actions=False,
        group_by="column",
        keepna=True,
        progress=False,
        threads=True,
    )

    if data.empty or "Close" not in data:
        raise RuntimeError("Yahoo returned no closing-price data")

    close_prices = data["Close"]
    missing_tickers = [
        ticker for ticker in YAHOO_TICKERS if ticker not in close_prices.columns
    ]
    if missing_tickers:
        raise RuntimeError(
            "Yahoo returned no data for: " + ", ".join(missing_tickers)
        )

    return close_prices.loc[:, list[Literal['AAPL', 'MSFT', 'NVDA', 'AMZN', 'GOOGL', 'META', 'AVGO', 'TSLA', 'JPM', 'LLY', 'MU', 'BRK-B', 'XOM', 'JNJ', 'WMT']](YAHOO_TICKERS)].sort_index()


def buildMarketModel(close_prices: pd.DataFrame) -> dict[str, object]:
    finite_prices = close_prices.to_numpy(dtype=float)
    observed_prices = finite_prices[~np.isnan(finite_prices)]
    if observed_prices.size == 0:
        raise ValueError("closing-price data contains no observations")
    if not np.isfinite(observed_prices).all() or (observed_prices <= 0.0).any():
        raise ValueError("closing prices must be finite and positive")

    complete_prices = close_prices.dropna(how="any")
    if complete_prices.empty:
        raise ValueError("no trading day contains prices for every symbol")

    log_returns = np.log(close_prices).diff().dropna(how="any")
    if len(log_returns) <= len(YAHOO_TICKERS):
        raise ValueError("not enough complete returns to estimate covariance")

    mean_log_returns = log_returns.mean(axis=0).to_numpy(dtype=float)
    covariance = log_returns.cov().to_numpy(dtype=float)
    covariance = (covariance + covariance.T) * 0.5

    if not np.isfinite(mean_log_returns).all():
        raise ValueError("mean log returns must be finite")
    if not np.isfinite(covariance).all():
        raise ValueError("covariance matrix must be finite")

    try:
        np.linalg.cholesky(covariance)
    except np.linalg.LinAlgError as error:
        raise ValueError(
            "estimated covariance is not positive definite"
        ) from error

    latest_prices = complete_prices.iloc[-1].to_numpy(dtype=float)
    as_of = pd.Timestamp(complete_prices.index[-1]).date().isoformat()

    return {
        "schema_version": 1,
        "as_of": as_of,
        "horizon_days": 1,
        "return_model": "lognormal",
        "symbols": list(MODEL_SYMBOLS),
        "prices": latest_prices.tolist(),
        "mean_log_returns": mean_log_returns.tolist(),
        "covariance": covariance.tolist(),
        "observations": int(len(log_returns)),
    }


def writeMarketModel(model: dict[str, object], output_path: Path) -> None:
    output_path.parent.mkdir(parents=True, exist_ok=True)
    with output_path.open("w", encoding="utf-8") as output_file:
        json.dump(model, output_file, indent=4, allow_nan=False)
        output_file.write("\n")


def main() -> None:
    arguments = parseArguments()
    close_prices = downloadClosePrices(arguments.start, arguments.end)
    model = buildMarketModel(close_prices)
    writeMarketModel(model, arguments.output)
    print(
        f"Wrote {arguments.output} with {model['observations']} observations "
        f"as of {model['as_of']}"
    )


if __name__ == "__main__":
    main()
