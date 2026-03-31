FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

WORKDIR /app

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    cmake \
    build-essential \
    libboost-all-dev \
    libasio-dev \
    git \
    wget \
    ca-certificates && \
    rm -rf /var/lib/apt/lists/*

# Copy project files
COPY . /app

# Download stable Crow header
RUN mkdir -p include && \
    wget https://github.com/CrowCpp/Crow/releases/download/v1.2.0/crow_all.h \
    -O include/crow_all.h

# Build project
RUN cmake -B build -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build --parallel

EXPOSE 8080

CMD ["./build/game_server"]