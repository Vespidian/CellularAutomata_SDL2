CC = gcc
CFLAGS = -Werror -Wall -lSDL2

compile:
	$(CC) main.c $(CFLAGS) -o CWGOL