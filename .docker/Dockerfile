ARG VERSION=edge
# this allows to work on forked repository
ARG REPOSITORY=greenbone/boreas
FROM greenbone/gvm-libs:$VERSION AS build

ARG DEBIAN_FRONTEND=noninteractive

COPY . /source
RUN sh /source/.github/install-dependencies.sh
RUN cmake -DCMAKE_BUILD_TYPE=Release -B/build /source

RUN DESTDIR=/install cmake --build /build -- install

FROM greenbone/gvm-libs:$VERSION

COPY --from=build /install/ /

RUN ldconfig
