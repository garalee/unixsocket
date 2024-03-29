CC=gcc

SRC = echocli.c timecli.c server.c client.c
OBJECTS = echocli.o timecli.o server.o client.o

TARGET = server client echocli timecli
LIBS = -pthread

all: server client

server : server.o
	$(CC) $^ -o $@ $(LIBS)
client : client.c echocli timecli
	$(CC) -g client.c -o $@ 

server.o : server.c
	$(CC) -c $^ -o $@

client.o : client.c
	$(CC) -g -c client.c -o $@

echocli: echocli.c
	$(CC) $^ -o $@ 

timecli: timecli.c
	$(CC) $^ -o $@ 

clean:
	rm *.o
	rm $(TARGET)
