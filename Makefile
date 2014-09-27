CC=gcc
LIBS = -lpthread


server : 
	$(CC) -o server server.c -g

client :
	$(CC) -o client client.c -g

clean : 
	rm client
	rm server