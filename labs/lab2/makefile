#George Delgado
#cs333

ASSIGNMENT= lab2

CC= gcc
DEBUG= -g3
CFLAGS= $(DEBUG) -Wall -Wshadow -Wunreachable-code -Wredundant-decls \
		-Wmissing-declarations -Wold-style-definition \
		-Wmissing-prototypes -Wdeclaration-after-statement \
		-Wno-return-local-addr -Wunsafe-loop-optimizations \
		-Wuninitialized -Werror -Wextra
LDFLAGS=
PERMS= g+x

PROG1=
PROG2=
PROG3=
PROG4=
PROGS= $(PROG1)#$(PROG2) $(PROG3) $(PROG4)

all: $(PROGS)

$(PROG1): $(PROG1).o
	$(CC) $(LDFLAGS) -o $(PROG1) $(PROG1).o

$(PROG1).o: $(PROG1).c
	$(CC) $(CFLAGS) -c $(PROG1).c

#$(PROG2): $(PROG2).o
#	$(CC) $(LDFLAGS) -o $(PROG2) $(PROG2).o
#
#$(PROG2).o: $(PROG2).c
#	$(CC) $(CFLAGS) -c $(PROG2).c
#
#$(PROG3): $(PROG3).o
#	$(CC) $(LDFLAGS) -o $(PROG3) $(PROG3).o
#
#$(PROG3).o: $(PROG3).c
#	$(CC) $(CFLAGS) -c $(PROG3).c
#
#$(PROG4): $(PROG4).o
#	$(CC) $(LDFLAGS) -o $(PROG4) $(PROG4).o
#
#$(PROG4).o: $(PROG4).c
#	$(CC) $(CFLAGS) -c $(PROG4).c

.PHONY: all clean tar

clean:
	rm -f $(PROGS) *.o *~ \#* #*.txt

TAR_FILE= ${LOGNAME}-$(ASSIGNMENT).tar.gz
tar: clean
	rm -f $(TAR_FILE)
	tar cvaf $(TAR_FILE) *.[c] [Mm]akefile
	tar tvaf $(TAR_FILE)
