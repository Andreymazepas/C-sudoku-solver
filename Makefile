CC=gcc
ODIR = obj
SDIR = src
INC = -Isrc -Ilib
CFLAGS= -Wall 
main:
	$(CC) $(INC) -g src/main.c -o p4 -lncurses $(CLAGS)
