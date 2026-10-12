//George Delgado
//cs333
//decrypt-w3-inode

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#define INPUT 'i'
#define OUTPUT 'o'
#define HELP 'h'
#define VERBOSE 'v'
#define MAX_BUFF_LEN 10000000 //I don't know what size to default to...
#define LOWER_THREE 3 //lower 3 bytes of inode id
#define NUM_BITS_8 8

int main(int argc, char *argv[])
{
    printf("argc: %d\nargv: %s\n", argc, argv[0]);
    
    return(EXIT_SUCCESS);
}
