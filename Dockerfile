# syntax=docker/dockerfile:1

ARG DEBIAN_VERSION=13.6-slim

FROM debian:${DEBIAN_VERSION} AS builder

RUN apt-get update \
    && apt-get install --yes --no-install-recommends \
        ca-certificates \
        cmake \
        g++ \
        git \
        ninja-build \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /source

COPY CMakeLists.txt ./
COPY apps/ apps/
COPY include/ include/
COPY src/ src/

RUN cmake \
        -S . \
        -B /build \
        -G Ninja \
        -DCMAKE_BUILD_TYPE=Release \
        -DHTPSTE_BUILD_TESTS=OFF \
    && cmake --build /build --target htpste --parallel \
    && /build/htpste --help

FROM debian:${DEBIAN_VERSION} AS runtime

RUN apt-get update \
    && apt-get install --yes --no-install-recommends libstdc++6 \
    && rm -rf /var/lib/apt/lists/* \
    && mkdir /data \
    && chown 10001:10001 /data

COPY --from=builder --chown=10001:10001 /build/htpste /usr/local/bin/htpste

WORKDIR /app
USER 10001:10001

ENTRYPOINT ["htpste"]
CMD ["--market-model", "/data/market_model.json"]
