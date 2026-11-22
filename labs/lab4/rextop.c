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
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "rextop.h"

int main(int argc, char *argv[])
{
    //bool is_verbose = false;
    {
        int opt = 0;
        while((opt = getopt(argc, argv, OPTIONS)) != -1)
        {
            switch(opt)
            {
                case PROC:
                    break;
                case INTERVAL:
                    break;
                case COUNT:
                    break;
                case WIDTH:
                    break;
                case VERBOSE:
                    //is_verbose = true;
                    break;
                case HELP:
                    help(argv[0]);
                    exit(EXIT_FAILURE);
                default:
                    help(argv[0]);
                    fprintf(stderr, "Invalid command line option...\n");
                    exit(EXIT_FAILURE);
                    break;
            }
        }
    }
}

void help(char *prog_name)
{
    printf("help text");
    printf("\n\tLab4/%s", prog_name);
    printf("\n\tOptions: %s", OPTIONS);
    printf("\n\t\t-m\t\tadd details");
    printf("\n\t\t-i #\t\tadd details");
    printf("\n\t\t-c #\t\tadd details");
    printf("\n\t\t-w #\t\tadd details");
    printf("\n\t\t-v\t\tgive verbose diagnostic messages");
    printf("\n\t\t-h\t\tdisplay this AMAZING help message");
    printf("\n");
}
