FROM debian:stretch-slim AS base
MAINTAINER Caian R. Ertl <hi@caian.org>

FROM base AS build
WORKDIR tmul
COPY Makefile ./
COPY src ./src

RUN apt-get update && apt-get install --no-install-recommends -y \
    build-essential \
    libedit-dev

RUN make

FROM base as deps
RUN apt-get update && apt-get install --no-install-recommends -y libedit-dev

FROM deps AS run
COPY --from=build tmul/tmul .
RUN mv tmul /usr/bin

ENTRYPOINT ["tmul"]
