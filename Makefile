.DEFAULT_GOAL := build

CC = cc
GDB = gdb

FILES = builtin.c eval.c fmt.c hash.c mpc.c parser.c prime.c repl.c
CFLAGS = -Wall -Wextra -pedantic -std=c99
LFLAGS = -ledit -lm

ARTIFACT = tmul


build:
	cd src && $(CC) $(CFLAGS) $(FILES) $(LFLAGS) -o $(ARTIFACT) \
		&& mv $(ARTIFACT) ../

clean:
	rm $(ARTIFACT)

dev: CFLAGS += -g
dev: build

debug: dev
	$(GDB) $(ARTIFACT)

install:
	mv $(ARTIFACT) /usr/bin

run:
	./$(ARTIFACT)

uninstall:
	rm /usr/bin/$(ARTIFACT)
