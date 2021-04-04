.PHONY: test-mpc test-lexy
.DEFAULT_GOAL := build

EFLAGS =
LFLAGS =

CFLAGS = -Wall -Wextra -Wno-unused-parameter
CFLAGS += $(EFLAGS)

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
	ARTIFACT = lexy.exe
else
	LFLAGS += -lm

	CFLAGS += -std=c99
	CFLAGS += -pedantic
endif



# base build target
build: $(MPC) $(LEXY_FILES)
	@printf "\nLEXY BUILD\n"
	@printf "\n>>> parameters\n"
	@printf "* CC: %s (%s)\n" "$(CC)" "$(shell which $(CC))"
	@printf "* EFLAGS: %s\n" "$(strip $(EFLAGS))"
	@printf "* LFLAGS: %s\n" "$(strip $(LFLAGS))"
	@printf "* CFLAGS: %s\n" "$(strip $(CFLAGS))"
	@printf "* PATH:\n" "$(PATH)"
	@echo "$(PATH)" | tr ':' '\n' | xargs -n 1 printf "   - %s\n"
	@printf "\n* readline found: "
	@if [ "$(MISSING_READLINE)" = "0" ]; then printf "yes"; else printf "no"; fi
	@printf "\n\n"
	@printf "\n>>> write-meta\n"
	CC="$(CC)" python3 utils/write-meta.py
	@printf "\n>>> compile\n"
	$(CC) $(CFLAGS) $^ $(LFLAGS) -o $(ARTIFACT)
	@printf "\n* binary size: "
	@du -h $(ARTIFACT) | cut -f -1
	@printf "\nDONE\n"

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
	@mv "$(ARTIFACT)" /usr/bin

uninstall:
	@rm "/usr/bin/$(ARTIFACT)"

run:
	@./$(ARTIFACT)

# clean the artifact and coverage-related files
clean:
	@rm -rf "$(ARTIFACT)" "$(ARTIFACT).dSYM" *.gcno *.gcda *.gcov
