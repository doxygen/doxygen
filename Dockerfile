FROM ubuntu:focal AS builder

RUN apt-get update && apt-get install -y \
    g++ \
    python \
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


FROM ubuntu:focal
RUN apt-get update && apt-get install --no-install-recommends -y \
    graphviz \
    && rm -rf /var/lib/apt/lists/*
COPY --from=builder /doxygen/build/bin/doxygen /usr/local/bin/
ENTRYPOINT ["doxygen"]
