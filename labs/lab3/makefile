#George Delgado
#gdelgado@pdx.edu
#cs333

ASSIGNMENT= lab3

#using rchaney directory for argon2.h
LOCAL_INCLUDES= -I /u/rchaney/Classes/cs333/Labs/Lab3/

CC= gcc
DEBUG= -g3
WERROR=
#WERROR+= -Werror
WFLAGS= -Wall -Wextra -Wshadow -Wunreachable-code -Wredundant-decls \
		-Wmissing-declarations -Wold-style-definition \
		-Wmissing-prototypes -Wdeclaration-after-statement \
		-Wno-return-local-addr -Wunsafe-loop-optimizations \
		-Wuninitialized $(WERROR)

CFLAGS= $(DEBUG) $(WFLAGS)
CFLAGS+= $(LOCAL_INCLUDES)
LDFLAGS=
#LDFLAGS+=
PERMS= g+x
PROG1= folio
PROG2=
PROG3=
PROG4=
PROGS= $(PROG1)

all: $(PROGS)

$(PROG1): $(PROG1).o
	$(CC) -o $(PROG1) $(PROG1).o $(LDFLAGS)

$(PROG1).o: $(PROG1).c
	$(CC) $(CFLAGS) -c $(PROG1).c -lmd

.PHONY: all clean tar test gdb val

FILE_TYPE=
FILE_TYPE+= *.o *.~ *.h
clean:
	rm -f $(PROGS) $(FILE_TYPE) \#*

TAR_FILE= ${LOGNAME}-$(ASSIGNMENT).tar.gz
tar: clean
	rm -f $(TAR_FILE)
	tar cvaf $(TAR_FILE) *.[c] [Mm]akefile
	tar tvaf $(TAR_FILE)

test: all
	./$(PROGS)-test.bash

ARGS=
#ARGS+=
gdb: all
	gdb -tui ./$(PROGS) $(ARGS)

val: all
	valgrind --leak-check=full --show-leak-kinds=all ./$(PROGS) $(ARGS)

