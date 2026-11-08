#George Delgado
#gdelgado@pdx.edu
#cs333

ASSIGNMENT= lab2

#using rchaney directory for argon2.h
LOCAL_INCLUDES= -I /u/rchaney/argon2/include
LOCAL_LIBS= -L /u/rchaney/argon2/lib/x86_64-linux-gnu -largon2

CC= gcc
DEBUG= -g3
WERROR=
WERROR+= -Werror
WFLAGS= -Wall $(WERROR) -Wshadow -Wunreachable-code \
		-Wredundant-decls -Wmissing-declarations \
		-Wold-style-definition -Wmissing-prototypes \
		-Wdeclaration-after-statement -Wno-return-local-addr \
		-Wunsafe-loop-optimizations -Wuninitialized \
		-Wno-unused-parameter -Wextra \
		-Wno-string-compare -Wno-stringop-overflow \
		-Wno-stringop-overread -Wno-stringop-truncation
CFLAGS= $(DEBUG) $(WFLAGS)
CFLAGS+= $(LOCAL_INCLUDES)
LDFLAGS=
LDFLAGS+= $(LOCAL_LIBS)
PERMS= g+x
PROG1= prince_of_aragon
PROG2=
PROG3=
PROG4=
PROGS= $(PROG1)

all: $(PROGS)

$(PROG1): $(PROG1).o
	$(CC) -o $(PROG1) $(PROG1).o $(LDFLAGS)

$(PROG1).o: $(PROG1).c
	$(CC) $(CFLAGS) -c $(PROG1).c

.PHONY: all clean tar val time test gdb sym small vim

UNWANTED=
#UNWANTED+= *.[SD]err *.[SD]out *.[SD]crack *.[SD]diff *.tot *.thds *.err *.out
#UNWANTED+= $(TXT)
#TXT= *.txt
HEADER=
#HEADER+= *.h
clean:
	rm -f $(PROGS) *.o *~ $(HEADER) $(UNWANTED) \#*

TAR_FILE= ${LOGNAME}-$(ASSIGNMENT).tar.gz
tar: clean
	rm -f $(TAR_FILE)
	tar cvaf $(TAR_FILE) *.[c] [Mm]akefile
	tar tvaf $(TAR_FILE)

ARGS= -v -h hashes-10.txt -p passwords-10.txt -l log.txt -o output.txt
val: all
	valgrind --leak-check=full --show-leak-kinds=all ./$(PROGS) $(ARGS)

time: all
	./time-test.bash

test: all
	./prince_of_aragon-test.bash

gdb: all
	gdb -tui ./prince-of-aragon $(ARGS)

#default
small: clean all sym
	./$(PROGS) $(ARGS)

#threads
smallt: clean all sym
	./$(PROGS) $(ARGS) -t 10

#threads and static
smalld: clean all sym
	./$(PROGS) $(ARGS) -t 10 -S

sym:
	ln -s /u/rchaney/Classes/cs333/Labs/Lab2/hashes-10.txt
	ln -s /u/rchaney/Classes/cs333/Labs/Lab2/passwords-10.txt

vim:
	vim $(PROGS).c
