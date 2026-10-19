#George Delgado
#cs333

ASSIGNMENT= vector

CC= gcc
DEBUG= -g
CFLAGS= $(DEBUG) -Wall
DNOISY= -DNOISY_DEBUG
LDFLAGS=
PERMS= og+x

PROG1= vector
PROG2=
PROG3=
PROGS= $(PROG1) $(PROG2) $(PROG3)

all: $(PROGS)

$(PROG1):
	$(CC) $(CFLAGS) $(DNOISY) -o $(PROG1) $(PROG1).c

.PHONY: all clean tar

clean:
	rm -f $(PROGS) *.o *~ \#*

TAR_FILE= ${LOGNAME}-$(ASSIGNMENT).tar.gz
tar: clean
	rm -f $(TAR_FILE)
	tar cvaf $(TAR_FILE) *.[c] #[Mm]akefile #do not include makefile for this version
	tar tvaf $(TAR_FILE)
