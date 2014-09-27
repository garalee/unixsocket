CC=gcc
CFLAGS = -W -Wall -g

SRC = echocli.c echoser.c timecli.c timeser.c server.c client.c
OBJECTS = echocli.o echoser.o timecli.o timeser.o server.o client.o

TARGET = server client
LIBS = -lpthread

$(TARGET) : server client

server : server.o
	$(CC) $(CFLAGS) $^ -o $@
client : client.o
	$(CC) $(CFLAGS) $^ -o $@

server.o : echoser timeser server.c
	$(CC) $(CFLAGS) -c server.c -o $@

client.o : echocli timecli client.c
	$(CC) $(CFLAGS) -c client.c -o $@

echoser: echoser.o
	$(CC) $(CFLAGS) $^ -o $@

echocli: echocli.o
	$(CC) $(CFLAGS) $^ -o $@

timeser: timeser.o
	$(CC) $(CFLAGS) $^ -o $@

timecli: timecli.
	$(CC) $(CFLAGS) $^ -o $@

timecli.o:
	$(CC) $(CFLAGS) -c $^ -o $@
echocli.o: 
	$(CC) $(CFLAGS) -c $^ -o $@

echoser.o: echoser.c
	$(CC) $(CFLAGS) -c $^ -o $@
timeser.o: timeser.c
	$(CC) $(CFLAGS) -c $^ -o $@