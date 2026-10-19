#George Delgado
#cs333

ASSIGNMENT= fib

CC= gcc
CFLAGS= -Wall
LDFLAGS=
GMP= -lgmp
PERMS= og+x

PROG1= fibr
PROG2= fibi
PROG3= fibg
PROGS= $(PROG1) $(PROG2) $(PROG3)

all: $(PROGS)

$(PROG1):
	$(CC) $(CFLAGS) $(PROG1).c -o $(PROG1)

$(PROG2):
	$(CC) $(CFLAGS) $(PROG2).c -o $(PROG2)

$(PROG3):
	$(CC) $(CFLAGS) $(PROG3).c -o $(PROG3) $(GMP)

.PHONY: all clean tar

clean:
	rm -f $(PROGS) *.o *~ \#*

TAR_FILE= ${LOGNAME}-$(ASSIGNMENT).tar.gz
tar: clean
	rm -f $(TAR_FILE)
	tar cvaf $(TAR_FILE) *.[c] #[Mm]akefile #do not include makefile for this version
	tar tvaf $(TAR_FILE)
