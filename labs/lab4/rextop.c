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
//man poll(2) used for poll
#include <poll.h>
#include "rextop.h"

int main(int argc, char *argv[])
{
    //bool is_verbose = false;
    int interval = DEFAULT_INTERVAL;
    int count = DEFAULT_COUNT;
    int width = DEFAULT_WIDTH;
    {
        int opt = 0;

        while((opt = getopt(argc, argv, OPTIONS)) != -1)
        {
            switch(opt)
            {
                case MONITOR:
                    break;
                case INTERVAL:
                    sscanf(optarg, "%d", &interval);
                    break;
                case COUNT:
                    sscanf(optarg, "%d", &count);
                    break;
                case WIDTH:
                    sscanf(optarg, "%d", &width);
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

    if(optind < argc)
    {
        fprintf(stderr, "\nToo many arguments provided...");
        exit(INVALID_CMD_OPTIONS);
    }
}

void help(char *prog_name)
{
    printf("Usage: %s -[%s]", prog_name, OPTIONS);
    printf("\n\t\t-m\t\tonly show your processes, instead of all processes");
    printf("\n\t\t-i #\t\tset the refresh interval, in seconds. Default is 2.");
    printf("\n\t\t-c #\t\tset the count of how many times the output will refressh before exiting. Default is 0 (don't stop)");
    printf("\n\t\t-w #\t\tset the width of texxt to display. Default is 100.");
    printf("\n\t\t-v\t\tminimal verbose output, to stderr");
    printf("\n\t\t-h\t\tdisplay this amazing help and exit");
    printf("\n");
}
