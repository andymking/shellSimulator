CC=gcc
CFLAGS=-c -Wall -g

all: thsh

thsh: shell.o driver.o
	$(CC) shell.o driver.o -o thsh

shell.o: shell.c shell.h
	$(CC) $(CFLAGS) shell.c

driver.o: driver.c shell.h
	$(CC) $(CFLAGS) driver.c

clean:
	/bin/rm -f thsh *.o *.gz

run: thsh
	./thsh

