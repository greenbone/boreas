ARG VERSION=unstable

FROM greenbone/gvm-libs:$VERSION

RUN apt-get update && apt-get install --no-install-recommends --no-install-suggests -y \
    build-essential \
    curl \
    cmake \
    pkg-config \
    libglib2.0-dev \
    libpcap-dev \
    && rm -rf /var/lib/apt/lists/*
