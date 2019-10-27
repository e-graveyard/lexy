.PHONY: test-mpc test-tmul
.DEFAULT_GOAL := build

CC = cc
GDB = gdb

CFLAGS = -Wall -Wextra -Wno-unused-parameter -pedantic -std=c99
LFLAGS = -ledit -lm
ARTIFACT = tmul

MPC = src/mpc.c
PTEST = tests/ptest.c

TMUL_FILES      = $(filter-out $(MPC), $(wildcard src/*.c))
MPC_TEST_FILES  = $(wildcard tests/mpc/*.c)
TMUL_TEST_FILES = $(wildcard tests/tmul/*.c)


build: $(MPC) $(TMUL_FILES)
	$(CC) $(CFLAGS) $^ $(LFLAGS) -o $(ARTIFACT)

install:
	mv $(ARTIFACT) /usr/bin

uninstall:
	rm /usr/bin/$(ARTIFACT)

run:
	./$(ARTIFACT)

clean:
	rm -f $(ARTIFACT)

debug: CFLAGS += -g
debug: build
	$(GDB) $(ARTIFACT)

test-mpc: $(PTEST) $(MPC) $(MPC_TEST_FILES)
	$(CC) $(CFLAGS) -Wno-unused $^ $(LFLAGS) -o $@ \
		&& ./$@; true \
		&& rm $@

test-tmul: $(PTEST) $(MPC) $(filter-out src/repl.c, $(TMUL_FILES)) $(TMUL_TEST_FILES)
	$(CC) $(CFLAGS) -Wno-unused $^ $(LFLAGS) -o $@ \
		&& ./$@; true \
		&& rm $@

test: test-mpc test-tmul
