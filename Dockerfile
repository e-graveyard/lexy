FROM debian:stretch-slim AS base
MAINTAINER Caian R. Ertl <hi@caian.org>

# -----
FROM base AS build
COPY Makefile .
COPY src src

RUN apt-get update && apt-get install --no-install-recommends -y \
    build-essential \
    libedit-dev

RUN make

# -----
FROM base as deps
RUN apt-get update && apt-get install --no-install-recommends -y libedit-dev

# -----
FROM deps AS run

WORKDIR lexy
COPY --from=build lexy lexy
COPY lib lib
CMD ./lexy
