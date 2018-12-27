.DEFAULT_GOAL := build

CC = gcc
LINKS = -ledit -lm
FLAGS = -Wall -Wextra -pedantic
STD = -std=c99
MPC = mpc.c

build:
	cd src && $(CC) parser.c $(MPC) $(LINKS) -o parser $(FLAGS) $(STD)

run:
	cd src && ./parser
