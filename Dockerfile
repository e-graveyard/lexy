# vi: ft=Dockerfile

FROM alpine:3.13 AS base
MAINTAINER Caian R. Ertl <hi@caian.org>


FROM base AS build-env
RUN apk add --no-cache \
    gcc \
    make \
    musl-dev \
    libedit-dev \
    python3


FROM build-env AS build
COPY Makefile .
COPY core core
COPY utils utils

RUN make build-release


FROM base AS deps
RUN apk add --no-cache libedit-dev


FROM deps AS run
COPY --from=build lexy lexy
COPY lib lib
ENTRYPOINT ["/lexy"]
