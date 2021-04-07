CC=g++
CFLAGS=-I.

all: P1

P1: P1.o
	$(CC) P1.o -o P1

clean: 
	-rm *.o
