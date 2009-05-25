CC=gcc
CFLAGS=-Wall -g
# FILES=$(SRC)/msh.c

# $< the name of the related file that caused the action.
# $* the prefix shared by target and dependent files. 

compile:
	@echo compiling!
	cd src && make

install:
	cd src && make install

clean:
	cd src && make clean
