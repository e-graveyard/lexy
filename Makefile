.DEFAULT_GOAL := build

build:
	cd tmul && $(CC) tmul.c -o tmul -Wall -Wextra -pedantic -std=c99

run:
	cd tmul && ./tmul
