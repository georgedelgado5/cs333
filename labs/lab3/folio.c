/*George Delgado
 * gdelgado@pdx.edu
 * cs333
 * lab3
*/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <stdbool.h>

//getopt options
#define EXTRACT 'x'
#define CREATE 'c'
#define S_TABLE 't'
#define L_TABLE 'T'
#define FILENAME 'f'
#define VALIDATE 'V'
#define VERBOSE 'v'
#define HELP 'h'
#define OPTIONS "xctTf:hvV"

//set default values -> use static if neccessary

//helper methods
void help(char *);

int main(int argc, char *argv[])
{
    bool is_verbose = false;
    {
        int opt = 0;
        while((opt = getopt(argc, argv, OPTIONS)) != -1)
        {
            switch(opt)
            {
                case EXTRACT:
                    //add method
                    break;
                case CREATE:
                    //add method`
                    break;
                case S_TABLE:
                    //set variable and call method that identifies long or short table
                    break;
                case L_TABLE:
                    //set variable and call method that identifies long or short table
                    break;
                case FILENAME:
                    //filename = optarg;
                    break;
                case VALIDATE:
                    //add method
                    break;
                case VERBOSE:
                    //is_verbose = true;
                    break;
                case HELP:
                default:
                    help(argv[0]);
                    exit(EXIT_SUCCESS);
                    break;
            }
        }
    }
    return(EXIT_SUCCESS);
}

//add open(), read(), write() file handlers
/*
 * Binary files require using:
 * int open(),
 * int read(),
 * int write()
*/

//other utilities to consider
/*
 * lseek() -> move through folio sections,
 * fork(),
 * exec() family of functions
*/

//print help message
void help(char *prog_name)
{
    printf("\nDescription:");
    printf("\nAdd description.\n");
    printf("\nUsage:\n");
    printf("\t%s [OPTION] [...]\n", prog_name);
    printf("Options:\n");
    printf("\t[-x] [...]\n");
    printf("\t[-c] [...]\n");
    printf("\t[-t] [...]\n");
    printf("\t[-T] [...]\n");
    printf("\t[-f] [...]\n");
    printf("\t[-V] [...]\n");
    printf("\t[-v] [...]\n");
    printf("\t[-h] [...]\n");
}
