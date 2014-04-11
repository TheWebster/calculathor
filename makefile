SHELL      = /bin/sh
CC         = /usr/bin/gcc

CFLAGS     = -Wall -O0 -g

HEADERS    = parser.h stack.h op.h
SOURCES    = parser.c stack.c op.c main.c

main: $(HEADERS) $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o main

