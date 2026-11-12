
# STAGE 1: Build the C++ worker
FROM fedora:44 AS builder

RUN dnf install -y \
    gcc-c++ \
    cmake \
    make \
    pkgconf-pkg-config \
    libpqxx-devel \
    hiredis-devel \
    openssl-devel \
    opencv-devel \
    libharu-devel \
    && dnf clean all

WORKDIR /app

COPY . .

RUN cmake -S . -B build
RUN cmake --build build -j$(nproc)


# STAGE 2: Runtime environment
FROM fedora:44 AS runtime

RUN dnf install -y \
    libpqxx \
    hiredis \
    openssl-libs \
    opencv \
    libharu \
    && dnf clean all

WORKDIR /app

COPY --from=builder /app/build/worker /usr/local/bin/worker

RUN mkdir -p /app/output

CMD ["/usr/local/bin/worker"]
