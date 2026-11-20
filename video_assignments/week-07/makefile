#George Delgado
#cs333

ASSIGNMENT= fortune

CC= gcc
DEBUG= -g3 -O0
CFLAGS= $(DEBUG) -Wall -Werror
LDFLAGS=
PERMS= g+rw,o+r

PROG1= fortune_client
PROG2= fortune_server
PROG3= 
PROG4= 
PROG5= 
PROGS= $(PROG1) $(PROG2)

all: $(PROGS)

$(PROG1): $(PROG1).o
	$(CC) $(LDFLAGS) -o $(PROG1) $(PROG1).o

$(PROG1).o: $(PROG1).c
	$(CC) $(CFLAGS) -c $(PROG1).c
	chmod $(PERMS) $(PROG1).c

$(PROG2): $(PROG2).o
	$(CC) $(LDFLAGS) -o $(PROG2) $(PROG2).o

$(PROG2).o: $(PROG2).c
	$(CC) $(CFLAGS) -c $(PROG2).c
	chmod $(PERMS) $(PROG2).c

# add git gdb valgrind test
.PHONY: all clean tar

FILETYPE= *.o *~
clean:
	rm -f $(PROGS) $(FILETYPE) \#*

TAR_FILE= ${LOGNAME}-$(ASSIGNMENT).tar.gz
tar: clean
	rm -f $(TAR_FILE)
	tar cvaf $(TAR_FILE) *.[c] #[Mm]akefile
	tar tvaf $(TAR_FILE)
