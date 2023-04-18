ARG VERSION=unstable

FROM greenbone/gvm-libs:$VERSION
LABEL deprecated="This image is deprecated and may be removed soon."

RUN apt-get update && apt-get install --no-install-recommends --no-install-suggests -y \
  build-essential \
  curl \
  cmake \
  pkg-config \
  libglib2.0-dev \
  libgnutls28-dev \
  libpcap-dev \
  && rm -rf /var/lib/apt/lists/*
