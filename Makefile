.PHONY: test-mpc test-lexy
.DEFAULT_GOAL := build

CC = cc

CFLAGS = -Wall -Wextra -Wno-unused-parameter -pedantic -std=c99
LFLAGS = -ledit -lm

# ---

MPC      = src/mpc.c
PTEST    = tests/ptest.c
ARTIFACT = lexy

LEXY_FILES      = $(wildcard src/*.c)
MPC_TEST_FILES  = $(wildcard tests/mpc/*.c)
LEXY_TEST_FILES = $(wildcard tests/lexy/*.c)

ifeq ($(OS),Windows_NT)
	ARTIFACT = 'lexy.exe'
endif


build: $(MPC) $(LEXY_FILES)
	@python3 scripts/write-meta.py
	$(CC) $(CFLAGS) $^ $(LFLAGS) -o $(ARTIFACT)

build-cov: CFLAGS += -coverage
build-cov: build

debug: CFLAGS += -g
debug: clean
debug: build

debug-gnu: debug
	gdb $(ARTIFACT)

debug-mac: debug
	lldb $(ARTIFACT)

test-mpc: $(PTEST) $(MPC) $(MPC_TEST_FILES)
	@$(CC) $(CFLAGS) -Wno-unused $^ $(LFLAGS) -o $@ \
		&& ./$@ \
		&& rm $@

test-lexy: $(PTEST) $(MPC) $(filter-out src/lexy.c, $(LEXY_FILES)) $(LEXY_TEST_FILES)
	@$(CC) $(CFLAGS) -Wno-unused $^ $(LFLAGS) -o $@ \
		&& ./$@ \
		&& rm $@

test: test-mpc test-lexy

install:
	@mv $(ARTIFACT) /usr/bin

uninstall:
	@rm /usr/bin/$(ARTIFACT)

run:
	@./$(ARTIFACT)

clean:
	@rm -f $(ARTIFACT) *.gcno *.gcda *.gcov
