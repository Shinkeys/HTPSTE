# HTPSTE — High-Throughput Portfolio Stress Testing Engine

[![CI](https://github.com/Shinkeys/HTPSTE/actions/workflows/ci.yml/badge.svg)](https://github.com/Shinkeys/HTPSTE/actions/workflows/ci.yml)

HTPSTE is a C++20 engine for reproducible portfolio stress testing. It loads an equity market model, generates correlated Monte Carlo shocks, reprices long and short positions, and calculates portfolio risk.

The goal is fast, validated simulation without losing reproducibility. The same market model and seed produce identical scenarios regardless of thread count.

## Current capabilities

- Load prices, mean log returns, and covariance from JSON.
- Validate model dimensions, values, symmetry, and positive definiteness.
- Generate correlated lognormal scenarios across multiple threads.
- Reprice long and short equity positions under each scenario.
- Calculate P&L, VaR, CVaR, and worst scenarios.
- Report runtime and scenarios per second.
- Run natively or as a non-root Docker container.
- Test changes with GoogleTest, ASan, UBSan, and GitHub Actions.

The core supports arbitrary portfolios. The current CLI creates a demonstration portfolio from market-model symbols; loading a real portfolio file is not implemented yet.

## Build

Requirements: C++20 compiler, CMake 3.20+, Ninja, Git.

```bash
cmake \
  -S . \
  -B build \
  -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DHTPSTE_BUILD_TESTS=ON

cmake --build build --parallel
```

## Run

```bash
./build/htpste \
  --market-model tests/fixtures/market_model.json \
  --scenarios 10000 \
  --seed 42 \
  --threads 4
```

Options:

```text
--market-model <path>  Market-model JSON
--scenarios <count>    Scenarios per job
--seed <value>         Random seed
--threads <count>      Generation threads
--repeat               Run until interrupted
--help                 Show help
```

Required market-model fields:

```json
{
  "symbols": ["AAPL", "MSFT"],
  "prices": [100.0, 200.0],
  "mean_log_returns": [0.001, -0.002],
  "covariance": [
    [0.04, 0.01],
    [0.01, 0.09]
  ]
}
```

## Market data

Generate a model from historical prices using NumPy, pandas, and yfinance:

```bash
python python/download_assets.py \
  --start 2022-01-01 \
  --output python/market_model.json
```

`python/market_model.json` is changing runtime data and is ignored by Git. `tests/fixtures/market_model.json` is fixed test data committed for reproducible CI.

## Docker

```bash
docker build --tag htpste:local .

docker run --rm \
  --mount type=bind,source="$PWD/python/market_model.json",target=/data/market_model.json,readonly \
  htpste:local \
  --market-model /data/market_model.json \
  --scenarios 100000 \
  --seed 42 \
  --threads 8
```

The multi-stage image uses stable Debian, builds with Ninja, and contains only the runtime executable and libraries.

## Tests and sanitizers

```bash
ctest --test-dir build --output-on-failure
```

Sanitized build:

```bash
CC=clang CXX=clang++ cmake \
  -S . \
  -B build-sanitized \
  -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DHTPSTE_BUILD_TESTS=ON \
  -DHTPSTE_ENABLE_SANITIZERS=ON

cmake --build build-sanitized --parallel
ctest --test-dir build-sanitized --output-on-failure
```

## Continuous integration

GitHub Actions runs on pull requests and pushes to `master`:

1. Release build and tests.
2. Debug build and tests under ASan and UBSan.
3. Docker build and simulation smoke test after both test jobs pass.

## Structure

```text
apps/                 CLI
include/htpste/       Public headers
src/                  Engine implementation
tests/                Tests and deterministic fixtures
python/               Market-model generator
.github/workflows/    CI configuration
```

HTPSTE is under development and is not intended for production trading, regulatory reporting, or financial advice.
