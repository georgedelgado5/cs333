#George Delgado
#cs333

ASSIGNMENT= tprimes

CC= gcc
DEBUG= -g3 -O0
CFLAGS= $(DEBUG) -Wall -Werror
LDFLAGS=
#PERMS= g+x

PROG1= tprimes1
PROG2= tprimes2
PROG3= tprimes3
PROG4= tprimes4
PROG5= tprimes5
PROGS= $(PROG1) $(PROG2) $(PROG3) $(PROG4) $(PROG5)

all: $(PROGS)

$(PROG1): $(PROG1).o
	$(CC) $(LDFLAGS) -o $(PROG1) $(PROG1).o

$(PROG1).o: $(PROG1).c
	$(CC) $(CFLAGS) -c $(PROG1).c

$(PROG2): $(PROG2).o
	$(CC) $(LDFLAGS) -o $(PROG2) $(PROG2).o

$(PROG2).o: $(PROG2).c
	$(CC) $(CFLAGS) -c $(PROG2).c

$(PROG3): $(PROG3).o
	$(CC) $(LDFLAGS) -o $(PROG3) $(PROG3).o

$(PROG3).o: $(PROG3).c
	$(CC) $(CFLAGS) -c $(PROG3).c

$(PROG4): $(PROG4).o
	$(CC) $(LDFLAGS) -o $(PROG4) $(PROG4).o

$(PROG4).o: $(PROG4).c
	$(CC) $(CFLAGS) -c $(PROG4).c

$(PROG5): $(PROG5).o
	$(CC) $(LDFLAGS) -o $(PROG5) $(PROG5).o

$(PROG5).o: $(PROG5).c
	$(CC) $(CFLAGS) -c $(PROG5).c

# add git gdb valgrind test
.PHONY: all clean tar

clean:
	rm -f $(PROGS) *.o *~ \#* #*.txt

TAR_FILE= ${LOGNAME}-$(ASSIGNMENT).tar.gz
tar: clean
	rm -f $(TAR_FILE)
	tar cvaf $(TAR_FILE) *.[c] #[Mm]akefile
	tar tvaf $(TAR_FILE)
