CC=gcc
CFLAGS=-Wall -g -lpthread -lrt
LDFLAGS=
BIN=mcryptlogger
SOURCES.c=mcryptlogger.c xorcrypt.c queue.c
OBJECTS= $(SOURCES.c:.c=.o)

mcryptlogger: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(BIN)

.c.o:
	$(CC) $(CFLAGS) -c $<

install:
	mkdir -p ../bin
	mv $(BIN) ../bin

clean:
	rm $(BIN) $(OBJECTS)

test-run:
	$(BIN)
