.DEFAULT_GOAL := build

CC = cc
CFLAGS = -Wall -Wextra -pedantic -std=c99
LFLAGS = -ledit -lm
FILES = tmul.c prompt.c parser.c mpc.c

build:
	cd src && $(CC) $(FILES) $(LFLAGS) -o tmul $(CFLAGS)

run:
	cd src && ./tmul
