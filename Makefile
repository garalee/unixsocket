all:
	gcc -o server server.c
	gcc -o client tcpcli.c