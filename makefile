SHELL      = /bin/sh
CC         = /usr/bin/gcc

CFLAGS     = -Wall -O0 -g

HEADERS    = calc.h stack.h op.h demo_symbols.h
SOURCES    = parser.c stack.c op.c demo.c demo_symbols.c

demo: $(HEADERS) $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o demo

