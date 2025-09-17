CC = gcc
CFLAGS = -Wall -g

all: server

server: main.o tools.o
	$(CC) $(CFLAGS) -o server main.o tools.o

tools.o: tools.c lib/common.h lib/tools.h
	$(CC) $(CFLAGS) -c tools.c

main.o: main.c lib/common.h lib/tools.h
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f *.o server