.DEFAULT_GOAL := build

CC = cc
GDB = gdb

FILES = builtin.c eval.c fmt.c hash.c mpc.c prime.c tmul.c parser.c
CFLAGS = -Wall -Wextra -pedantic -std=c99
LFLAGS = -ledit -lm

ARTIFACT = tmul

build:
	cd src && $(CC) $(CFLAGS) $(FILES) $(LFLAGS) -o $(ARTIFACT)

dev: CFLAGS += -g
dev: build

debug: dev
	cd src && $(GDB) $(ARTIFACT)

run:
	cd src && ./$(ARTIFACT)
