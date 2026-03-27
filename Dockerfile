FROM ubuntu:jammy AS builder

RUN apt-get update && apt-get install -y \
    g++ \
    python3 \
    cmake \
    flex \
    bison \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /doxygen
COPY . .

RUN mkdir build \
    && cd build \
    && cmake -G "Unix Makefiles" .. \
    && make \
    && make install


FROM ubuntu:jammy
RUN apt-get update && apt-get install --no-install-recommends -y \
    graphviz \
    && rm -rf /var/lib/apt/lists/*
COPY --from=builder /doxygen/build/bin/doxygen /usr/local/bin/
WORKDIR /doxygen
ENTRYPOINT ["doxygen"]
