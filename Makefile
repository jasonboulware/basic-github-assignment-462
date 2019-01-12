CC=gcc
CFLAGS=-c -Wall -g #-std=c11

all: hw1

hw1: memory.o cache.o hw1.o
	$(CC) memory.o cache.o hw1.o -o hw1 -lm

memory.o: memory.c
	$(CC) $(CFLAGS) memory.c

cache.o: cache.c
	$(CC) $(CFLAGS) cache.c

hw1.o: hw1.c
	$(CC) $(CFLAGS) hw1.c

clean:
	/bin/rm -f hw1 *.o *.gz

run:
	./hw1 memory.txt

tarball:
	tar -cvzf Boulware.tar.gz memory.c cache.c

