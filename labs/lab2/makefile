#George Delgado
#gdelgado@pdx.edu
#cs333

ASSIGNMENT= lab2

CC= gcc
DEBUG= -g3
#WERROR= -Werror
WFLAGS= -Wall $(WERROR) -Wshadow -Wunreachable-code \
		-Wredundant-decls -Wmissing-declarations \
		-Wold-style-definition -Wmissing-prototypes \
		-Wdeclaration-after-statement -Wno-return-local-addr \
		-Wunsafe-loop-optimizations -Wuninitialized -Werror \
		-Wno-unused-parameter \
		-Wno-string-compare -Wno-stringop-overflow \
		-Wno-stringop-overread -Wno-stringop-truncation
CFLAGS= $(DEBUG) $(WFLAGS)
LDFLAGS=
PERMS= g+x

PROG1= prince_of_aragon
PROG2=
PROG3=
PROG4=
PROGS= $(PROG1)

all: $(PROGS)

$(PROG1): $(PROG1).o
	$(CC) $(LDFLAGS) -o $(PROG1) $(PROG1).o

$(PROG1).o: $(PROG1).c
	$(CC) $(CFLAGS) -c $(PROG1).c

.PHONY: all clean tar

clean:
	rm -f $(PROGS) *.o *~ *.h *.txt *.[SD]err *.[SD]out \#*

TAR_FILE= ${LOGNAME}-$(ASSIGNMENT).tar.gz
tar: clean
	rm -f $(TAR_FILE)
	tar cvaf $(TAR_FILE) *.[c] [Mm]akefile
	tar tvaf $(TAR_FILE)
