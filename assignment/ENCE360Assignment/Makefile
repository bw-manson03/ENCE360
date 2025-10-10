# Compiler and flags
CC = mpicc
CFLAGS = -Wall -Werror -O2 -lm -lpthread

.PHONY: default all clean

default: serial thread process processThread
all: default

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

serial: serial.c
	$(CC) -o $@ $^ $(CFLAGS)

thread: thread.c
	$(CC) -o $@ $^ $(CFLAGS)

process: process.c
	$(CC) -o $@ $^ $(CFLAGS)

processThread: processThread.c
	$(CC) -o $@ $^ $(CFLAGS)

run_serial: serial
	./serial

run_thread: thread
	./thread

run_process: process
	./process

run_processThread: processThread
	./processThread

clean:
	-rm -f serial thread process processThread *.o