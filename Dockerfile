# Use a modern base with build tools
FROM ubuntu:22.04

# Set noninteractive for apt
ENV DEBIAN_FRONTEND=noninteractive

# Update and install required packages
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    wget \
    curl \
    unzip \
    pkg-config \
    libpthread-stubs0-dev \
    libspdlog-dev \
    && rm -rf /var/lib/apt/lists/*

# Create app directory
WORKDIR /app

# Copy project files
COPY . .

# Configure & build (Release mode)
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build -j$(nproc)

# Run tests automatically when building (optional)
RUN cd build && ctest --output-on-failure || true

# Default entrypoint: run your streamer
CMD ["./build/pi-camera-streamer"]
