//George Delgado
//cs333
//decrypt-v-symlink
//
//Program reads a file pointed to by a symbolic link, and decrypts the data into an output file or stdout.
//
//The decryption key (also the encryption key) is the name of the **symbolic link** from the input file.
//
//unlink(2) - delete a name (and possibly a file) it refers to
//
//A symbolic-link is a directory entry that maps a name to a path.
//The symbolic link is a pointer to another directory entry.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#define OUTPUT 'o'
#define SYMBOLIC 's'
#define HELP 'h'
#define VERBOSE 'v'
#define OPTIONS "o:s:vh"
#define MAX_BUFF_LEN 10000000 //I don't know what size to default to...
#define NUM_BITS_8 8

void help(char *);

int main(int argc, char *argv[])
{
    printf("argc: %d\nargv: %s\n", argc, argv[0]);
    
    return(EXIT_SUCCESS);
}

//print help message
//argument is program name -> argv[0]
void help(char *prog_name)
{
    printf("Description:");
    printf("\nReads an input file pointed to by a symbolic link, and decrypts contents to an output file or stdout.\n");
    printf("\nUsage:\n");
    printf("\t%s [OPTION]\n", prog_name);
    printf("Options:\n");
    printf("\t[-o] [FILE] The name of the output file.\n");
    printf("\t[-s] [FILE] The name of the symbolic link that points to the encrypted file. (Required)\n");
    printf("\t[-h] Print help information.\n");
    printf("\t[-v] Describe process being performed.\n");
}
