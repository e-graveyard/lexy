.DEFAULT_GOAL := build

CC = gcc
LINKS = -ledit
FLAGS = -Wall -Wextra -pedantic
STD = -std=c99

build:
	cd src && $(CC) tmul.c $(LINKS) -o tmul $(FLAGS) $(STD)

run:
	cd src && ./tmul
