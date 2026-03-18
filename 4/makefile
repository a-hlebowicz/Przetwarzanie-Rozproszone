CC=gcc
CFLAGS=-Wall -fPIC -O2
LDFLAGS=-ldl

all: mastermind libdekoder.so

mastermind: mastermind.c
	$(CC) mastermind.c -o mastermind $(LDFLAGS)

libdekoder.so: dekoder.c dekoder.h
	$(CC) $(CFLAGS) -shared dekoder.c -o libdekoder.so

clean:
	rm -f mastermind libdekoder.so
	rm -rf .vscode