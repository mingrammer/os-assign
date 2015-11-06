CC=gcc
CFLAGS=-l.

simulator: simulator.o
	$(CC) -o simulator simulator.o

clean:
	rm simulator.o simulator
