.PHONY: test-mpc test-lexy
.DEFAULT_GOAL := build

CFLAGS = -Wall -Wextra -Wno-unused-parameter
LFLAGS =

# ---

MPC      = core/mpc.c
PTEST    = tests/ptest.c
ARTIFACT = lexy

LEXY_FILES      = $(wildcard core/*.c)
MPC_TEST_FILES  = $(wildcard tests/mpc/*.c)
LEXY_TEST_FILES = $(wildcard tests/lexy/*.c)

MISSING_READLINE = $(shell CC="$(CC)" utils/has-readline.sh)

# 0 = FALSE, 1 = TRUE
ifeq ($(MISSING_READLINE),0)
	# links editline if the lib is installed & available
	LFLAGS += -ledit
endif

ifeq ($(OS),Windows_NT)
	ARTIFACT = 'lexy.exe'
else
	LFLAGS += -lm

	CFLAGS += -std=c99
	CFLAGS += -pedantic
endif


# base build target
build: $(MPC) $(LEXY_FILES)
	@printf "\nLEXY PRE-BUILD\n\n"
	@printf "* PATH: %s\n" "$(PATH)"
	@printf "* CC: %s\n" "$(CC)"
	@printf "* readline found: "
	@if [ "$(MISSING_READLINE)" = "0" ]; then printf "yes"; else printf "no"; fi
	@printf "\n\n"
	@CC="$(CC)" python3 utils/write-meta.py
	$(CC) $(CFLAGS) $^ $(LFLAGS) -o $(ARTIFACT)

# build with optimizations (binary release)
build-release: CFLAGS += -Os
build-release: build

# build and generate coverage files
build-cov: CFLAGS += -coverage
build-cov: clean build

# build more debuggable files
debug: CFLAGS += -g
debug: build

# build debug and run with gdb (GCC toolchain)
debug-gcc: debug
	gdb $(ARTIFACT)

# build debug and run with lldb (LLVM toolchain)
debug-llvm: debug
	lldb $(ARTIFACT)

# compile test suite for MPC and run
test-mpc: $(PTEST) $(MPC) $(MPC_TEST_FILES)
	@$(CC) $(CFLAGS) -Wno-unused $^ $(LFLAGS) -o $@ \
		&& ./$@ \
		&& rm $@

# compile test suite for lexy and run
test-lexy: $(PTEST) $(MPC) $(filter-out core/lexy.c, $(LEXY_FILES)) $(LEXY_TEST_FILES)
	@$(CC) $(CFLAGS) -Wno-unused $^ $(LFLAGS) -o $@ \
		&& ./$@ \
		&& rm $@

# run all tests
test: test-mpc test-lexy

install:
	@mv $(ARTIFACT) /usr/bin

uninstall:
	@rm /usr/bin/$(ARTIFACT)

run:
	@./$(ARTIFACT)

# clean the artifact and coverage-related files
clean:
	@rm -f $(ARTIFACT) *.gcno *.gcda *.gcov
