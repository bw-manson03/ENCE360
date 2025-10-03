# Compiler and flags
CC = mpicc
CFLAGS = -Wall -Werror -O2 -lm -lpthread

.PHONY: default all clean

default: serial thread
all: default

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

serial: serial.c
	$(CC) -o $@ $^ $(CFLAGS)

thread: thread.c
	$(CC) -o $@ $^ $(CFLAGS)

run_serial: serial
	./serial

run_thread: thread
	./thread

clean:
	-rm -f serial thread *.o