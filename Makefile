CC=gcc

SRC = echocli.c timecli.c server.c client.c
OBJECTS = echocli.o timecli.o server.o client.o

TARGET = server client echocli timecli
LIBS = -pthread

server : server.o
	$(CC) $^ -o $@ $(LIBS)
client : client.c echocli timecli
	$(CC) client.c -o $@ $(LIBS)

server.o : server.c
	$(CC) -c $^ -o $@

client.o : client.c
	$(CC) -c client.c -o $@

echocli: echocli.c
	$(CC) $^ -o $@ $(LIBS)

timecli: timecli.c
	$(CC) $^ -o $@ $(LIBS)


clean:
	rm *.o
	rm $(TARGET)
