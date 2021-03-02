.PHONY: test-mpc test-lexy
.DEFAULT_GOAL := build

CC = cc

CFLAGS = -Wall -Wextra -Wno-unused-parameter -pedantic -std=c99
LFLAGS = -ledit -lm
ARTIFACT = lexy

MPC = src/mpc.c
PTEST = tests/ptest.c

LEXY_FILES      = $(filter-out $(MPC), $(wildcard src/*.c))
MPC_TEST_FILES  = $(wildcard tests/mpc/*.c)
LEXY_TEST_FILES = $(wildcard tests/lexy/*.c)


build: $(MPC) $(LEXY_FILES)
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
debug: clean
debug: build

debug-gnu: debug
	gdb $(ARTIFACT)

debug-mac: debug
	lldb $(ARTIFACT)

test-mpc: $(PTEST) $(MPC) $(MPC_TEST_FILES)
	$(CC) $(CFLAGS) -Wno-unused $^ $(LFLAGS) -o $@ \
		&& ./$@; true \
		&& rm $@

test-lexy: $(PTEST) $(MPC) $(filter-out src/lexy.c, $(LEXY_FILES)) $(LEXY_TEST_FILES)
	$(CC) $(CFLAGS) -Wno-unused $^ $(LFLAGS) -o $@ \
		&& ./$@; true \
		&& rm $@

test: test-mpc test-lexy
