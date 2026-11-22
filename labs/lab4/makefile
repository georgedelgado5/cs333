#George Delgado
#gdelgado@pdx.edu
#cs333

#rextop
ASSIGNMENT= lab4

CC= gcc
DEBUG= -g3
WERROR=
#WERROR+= -Werror
WFLAGS= -Wall -Wextra -Wshadow -Wunreachable-code \
		-Wredundant-decls -Wmissing-declarations \
		-Wold-style-definition -Wmissing-prototypes \
		-Wdeclaration-after-statement -Wreturn-mismatch \
		-Wreturn-type -Wswitch-enum -Wswitch-default \
		-Wunused-const-variable -Wunsafe-loop-optimizations \
		-Wuninitialized $(WERROR) -Wno-unused-parameter

CFLAGS= $(DEBUG) $(WFLAGS)
CFLAGS+=
LDFLAGS=
LDFLAGS+=
PERMS= g+rw,o+r
PROG1= rextop
PROG2=
PROG3=
PROG4=
PROGS= $(PROG1)

all: $(PROGS)

$(PROG1): $(PROG1).o
	$(CC) -o $(PROG1) $(PROG1).o $(LDFLAGS)
	chmod $(PERMS) $(PROGS).c

$(PROG1).o: $(PROG1).c $(PROG1).h
	$(CC) $(CFLAGS) -c $(PROG1).c

.PHONY: all clean tar test gdb val

FILE_TYPE= *.o *.~
#FILE_TYPE+= *.h
clean:
	rm -f $(PROGS) $(FILE_TYPE) \#*

TAR_FILE= ${LOGNAME}-$(ASSIGNMENT).tar.gz
tar: clean
	rm -f $(TAR_FILE)
	tar cvaf $(TAR_FILE) $(PROGS).[c] [Mm]akefile
	tar tvaf $(TAR_FILE)

test: all
	./$(PROGS)-test.bash

ARGS=
ARGS+=
gdb: all

val: all
