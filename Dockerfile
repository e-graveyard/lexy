FROM debian:stretch-slim
MAINTAINER Caian R. Ertl <hi@caian.org>

WORKDIR tmul
COPY Makefile ./
COPY src ./src

RUN apt-get update && apt-get install --no-install-recommends -y \
    build-essential \
    libedit-dev

RUN rm -rf /var/lib/apt/lists/*
RUN make
RUN mv src/tmul /usr/bin

ENTRYPOINT ["tmul"]
