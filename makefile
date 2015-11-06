CC=gcc
CFLAGS=-l.

simulator: simulator.o
	$(CC) -o simulator simulator.o -lpthread

clean:
	rm simulator.o simulator
