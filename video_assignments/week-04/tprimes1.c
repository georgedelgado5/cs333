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
#define OPTIONS "u:h"

static long upper_bound = 0;
static void *find_primes(long);
void help(char *);

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
            default:
                help(argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    
    if(optind < argc)
    {
        fprintf(stderr, "Valid option must be provided.\n");
        help(argv[0]);
        exit(EXIT_FAILURE);
    }

    if(upper_bound == 0)
    {
        fprintf(stderr, "Upper bound option [-u Num] is required.\n");
        help(argv[0]);
        exit(EXIT_FAILURE);
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
            for(long factor = 2; factor < candidate; ++factor)
            {
                if((candidate % factor) == 0)
                {
                    is_prime = false;
                    break;
                }
            }
            if(is_prime)
            {
                printf("%ld\n", candidate);
                found++;
            }
        }
        candidate +=1;
    }
    fprintf(stderr, "%2ld  %8ld\n", begin, found);

    exit(EXIT_SUCCESS);
}

//print help message
//argument is program name -> argv[0]
void help(char *prog_name)
{
    printf("\nUsage:\n");
    printf("\t%s [OPTION]\n", prog_name);
    printf("Options:\n");
    printf("\t[-u NUM] Number of upper bound for largest prime we desire.\n");
    printf("\t[-h] Print help information.\n");
}
