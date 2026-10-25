//George Delgado
//gdelgado@pdx.edu
//cs333
//tprimes2.c

//gcc -g3 -O0 -Wall -Werror -o tprimes2 tprimes2.c

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
#define NS_PER_SECOND 1000000000.0
// this is a 1 and 9-zeroes, followed by a decimal point and a zero

static long upper_bound = 0;
static void *find_primes(long);
static double elapse_time(struct timespec *, struct timespec *);
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
    struct timespec et0;
    struct timespec et1;
    double et = 0.0;

    clock_gettime(CLOCK_MONOTONIC, &et0);

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
    clock_gettime(CLOCK_MONOTONIC, &et1);

    et = elapse_time(&et0, &et1);
    fprintf(stderr, "%2ld  %8ld et: %10.4lf\n", begin, found, et);
    
    exit(EXIT_SUCCESS);
}

static double elapse_time(struct timespec *t0, struct timespec *t1)
{
    double et = (t1->tv_sec - t0->tv_sec)
        + ((t1->tv_nsec - t0->tv_nsec) / NS_PER_SECOND);

    return et;
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
