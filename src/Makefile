CC=gcc
CFLAGS=-std=c99 -Wall -g #-lpthread #-lrt
LDFLAGS=
BIN=mcryptlogger
SOURCES.c=mcryptlogger.c xorcrypt.c queue.c
OBJECTS= $(SOURCES.c:.c=.o)

mcryptlogger: $(OBJECTS)
	@echo compiling mdu!
	$(CC) $(CFLAGS) $(OBJECTS) -o $(BIN)

.c.o:
	@echo .c.o wtf
	$(CC) $(CFLAGS) -c $<

install:
	@echo Installing msh
	mkdir -p ../bin
	mv $(BIN) ../bin

clean:
	@echo cleaning mdu!
	rm $(BIN) $(OBJECTS)

test-run:
	@echo testrun: -------------------------
	$(BIN)
