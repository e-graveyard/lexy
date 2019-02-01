.PHONY: tests
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

install:
	mv $(ARTIFACT) /usr/bin

uninstall:
	rm /usr/bin/$(ARTIFACT)

run:
	./$(ARTIFACT)

clean:
	rm $(ARTIFACT)

debug: CFLAGS += -g
debug: build
	$(GDB) $(ARTIFACT)

tests: CFLAGS += -Wno-unused
tests:
	cd tests && $(CC) $(CFLAGS) suite.c ptest.c \
		../src/fmt.c \
		$(LFLAGS) -o $@ \
		&& ./$@; true
