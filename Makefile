CC = gcc
CFLAGS = -Werror -Wall -lSDL2

compile:
	$(CC) $(CFLAGS) -o CWGOL.exe main.c