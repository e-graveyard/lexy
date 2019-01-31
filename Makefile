.DEFAULT_GOAL := build

CC = cc
GDB = gdb

FILES = builtin.c eval.c fmt.c hash.c mpc.c parser.c prime.c repl.c
CFLAGS = -Wall -Wextra -pedantic -std=c99
LFLAGS = -ledit -lm

ARTIFACT = tmul


build:
	cd src && $(CC) $(CFLAGS) $(FILES) $(LFLAGS) -o $(ARTIFACT)

clean:
	cd src && rm $(ARTIFACT)

dev: CFLAGS += -g
dev: build

debug: dev
	cd src && $(GDB) $(ARTIFACT)

install:
	cd src && mv $(ARTIFACT) /usr/bin

run:
	cd src && ./$(ARTIFACT)

uninstall:
	rm /usr/bin/$(ARTIFACT)
