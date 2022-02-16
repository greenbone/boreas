ARG VERSION=unstable
# this allows to work on forked repository
ARG REPOSITORY=greenbone/boreas
FROM ${REPOSITORY}-build:$VERSION AS build

ARG DEBIAN_FRONTEND=noninteractive

COPY . /source
RUN cmake -DCMAKE_BUILD_TYPE=Release -B/build /source
RUN DESTDIR=/install cmake --build /build -- install 

FROM greenbone/gvm-libs:$VERSION

COPY --from=build /install/ /

RUN ldconfig
