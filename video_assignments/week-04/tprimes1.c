//George Delgado
//gdelgado@pdx.edu
//cs333
//tprimes1.c

//gcc -g3 -O0 -Wall -Werror -o tprimes1 tprimes1.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <getopt.h>
#include <stdbool.h>

#define UPPER 'u'
#define HELP 'h'
#define OPTIONS "u:vh"

static long upper_bound = 0;
static void *find_primes(long);

int main(int argc, char *argv[])
{
    int opt = 0; 
    
    //getopt
    while((opt = getopt(argc, argv, OPTIONS)) != -1)
    {
        switch(opt)
        {
            case UPPER:
                {
                    int ret = sscanf(optarg, "%ld", &upper_bound);

                    if(ret != 1)
                    {
                        fprintf(stderr, "invalid value for upper bound %s\n", optarg);
                        exit(EXIT_FAILURE);
                    }
                }
                break;
            case HELP:
                help(argv[0]);
                exit(EXIT_SUCCESS);
                break;
            default:
                break;
        }
    }

    if(upper_bound < 0)
    {
        fprintf(stderr, "invalid value for upper bound %ld\n", upper_bound);
        exit(EXIT_FAILURE);
    }

    find_primes(0);

    exit(EXIT_SUCCESS);
}

static void *find_primes(long begin)
{
    long candidate = begin;
    long found = 0;

    while(candidate < upper_bound)
    {
        bool is_prime = true;
        
        if(candidate > 1)
        {
            for(log factor = 2; factor < candidate; ++factor)
            {
                if((candidate % factor) == 0)
                {
                    is_prime = false;
                    break;
                }
            }
        }
    }
}

//print help message
//argument is program name -> argv[0]
void help(char *prog_name)
{
    printf("Description:");
    printf("\nReads an input file or stdin, and encrypts contents to an output file.\n");
    printf("\nUsage:\n");
    printf("\t%s [OPTION]\n", prog_name);
    printf("Options:\n");
    printf("\t[-u NUM] Number of upper bound for largest prime we 
    printf("\t[-h] Print help information.\n");
}
