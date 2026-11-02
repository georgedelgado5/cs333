//George Delgado
//gdelgado@pdx.edu
//cs333
//lab2 - prince of aragon

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <pthreads.h>

// getopt options
#define HASHES 'h'
#define PLAIN 'p'
#define OUTPUT 'o'
#define LOGFILE 'l'
#define THREADS 't'
#define STATIC 'S'
#define VERBOSE 'v'
#define HELP 'H'
#define OPTIONS "h:p:olt::SvH"

// thread requirements
#define DEFAULT_THREADS 1
#define MAX_THREADS 16

void help(char *);

int main(int argc, char *argv[])
{
    bool is_verbose = false;
    int opt = 0;
 
    if(argc < 2)
    {
        fprintf(stderr, "No options provided...\n");
        help(argv[0]);
        exit(EXIT_FAILURE);
    }
 
    //getopt
    while((opt = getopt(argc, argv, OPTIONS)) != -1)
    {
        switch(opt)
        {
            case HASHES:
                break;
            case PLAIN:
                break;
            case OUTPUT:
                break;
            case LOGFILE:
                break;
            case THREADS:
                break;
            case STATIC:
                break;
            case VERBOSE:
                is_verbose = true;
                break;
            case HELP:
                help(argv[0]);
                exit(EXIT_SUCCESS);
                break;
            default:
                break;
        }
    }

    // Thoughts on what needs to happen next
    // - use read and write for input/output file
    //  - Important: use a 1000 char buffer; use a while loop when reading/writing
    // - use pthreads
    //  - mutex for keeping threads and for concurrency
    //  
    //
    //  1. Input/Output
    //      - match input hash files with corresponding password files
    //      - send to stdout or output file
    //          - output should be two options cracked and failed
    //
    //  - use argon2_verify() to 
    
    exit(EXIT_SUCCESS);
}

//print help message
//argument is program name -> argv[0]
void help(char *prog_name)
{
    printf("Description:");
    printf("\nAdd description.\n");
    printf("\nUsage:\n");
    printf("\t%s [OPTION]\n", prog_name);
    printf("Options:\n");
    printf("\t[-h] Specify the name of the input file of argon2 hashes.\n");
    printf("\t[-p] Specify the name of the input file of plaintext passwords.\n");
    printf("\t[-o] Specify the name of the output file. Defaults to stdout.\n");
    printf("\t[-l] Specify the name of the log file. Defaults to stderr.\n");
    printf("\t[-t] Specify the number of threads to use. Defaults to '1' thread.\n");
    printf("\t[-S] Run with static work allocation. Defaults to dynamic work allocation.\n");
    printf("\t[-v] Describe process being performed.\n");
    printf("\t[-H] Print help information.\n");
}
