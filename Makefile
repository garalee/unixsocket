CC=gcc
LIBS = -lpthread


all:
	gcc -o server server.c
	gcc -o client tcpcli.c

server : server.o
	$(CC) gcc