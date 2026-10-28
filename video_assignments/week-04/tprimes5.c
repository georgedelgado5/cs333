//George Delgado
//gdelgado@pdx.edu
//cs333
//tprimes5.c

//gcc -g3 -O0 -Wall -Werror -o tprimes5 tprimes5.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <getopt.h>
#include <stdbool.h>

#define NUM_THREADS 't'
#define UPPER 'u'
#define HELP 'h'
#define OPTIONS "t:u:h"
#define NS_PER_SECOND 1000000000.0
#define MAX_THREADS 30
// this is a 1 and 9-zeroes, followed by a decimal point and a zero

static long upper_bound = 0;
static int num_threads = 0;

// We are no longer using these variables as global access
//static long next_candidate = 2; // shared resource
//static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; // this is how we mediate access to shared resource

static void *find_primes(void *arg);
static double elapse_time(struct timespec *, struct timespec *);
static long get_next_candidate(void);
void help(char *);

int main(int argc, char *argv[])
{
    pthread_t *workers = NULL;
    {
        int opt = 0; 
        
        //getopt
        while((opt = getopt(argc, argv, OPTIONS)) != -1)
        {
            switch(opt)
            {
                case NUM_THREADS:
                    {
                        int ret = sscanf(optarg, "%d", &num_threads);

                        if(ret != 1)
                        {
                            fprintf(stderr, "invalid value for upper bound %s\n", optarg);
                            exit(EXIT_FAILURE);
                        }
                    }
                    break;
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

    if((num_threads < 0) || (num_threads > MAX_THREADS))
    {
        fprintf(stderr, "invalid value for number of threads %d\n", num_threads);
        exit(EXIT_FAILURE);
    }

    workers = malloc(num_threads * (sizeof(pthread_t)));

    for(long tid = 0; tid < num_threads; ++tid)
    {
        int ret = pthread_create(
                &(workers[tid])
                , NULL
                , find_primes
                , (void *) tid
                );
        if(ret != 0)
        {
            perror("thrad creation failed");
            fprintf(stderr, "thread creation failed on %ld\n", tid);
            exit(EXIT_FAILURE);
        }
    }

    for(int tid = 0; tid < num_threads; ++tid)
    {
        pthread_join(workers[tid], NULL);
    }

    free(workers);

    pthread_exit(NULL);
}

static void *find_primes(void *arg)
{
    long begin = ((long) arg);
    long candidate = begin;
    long found = 0;
    struct timespec et0;
    struct timespec et1;
    double et = 0.0;

    clock_gettime(CLOCK_MONOTONIC, &et0);

    while(candidate < upper_bound)
    {
        bool is_prime = true;

        candidate = get_next_candidate();

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
    }
    clock_gettime(CLOCK_MONOTONIC, &et1);

    et = elapse_time(&et0, &et1);
    fprintf(stderr, "%2ld  %8ld et: %10.4lf\n", begin, found, et);
    
    pthread_exit(NULL);
}

static double elapse_time(struct timespec *t0, struct timespec *t1)
{
    double et = (t1->tv_sec - t0->tv_sec)
        + ((t1->tv_nsec - t0->tv_nsec) / NS_PER_SECOND);

    return et;
}

static long get_next_candidate(void)
{
    // By setting the static variables insdie this function, we can protect the variable 'next_candidate'and lock
    static long next_candidate = 2;
    static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
    long candidate = 0; // local variable unique to each call

    // critical section
    pthread_mutex_lock(&lock);
    candidate = next_candidate;
    next_candidate++;
    pthread_mutex_unlock(&lock);

    return candidate;
}

//print help message
//argument is program name -> argv[0]
void help(char *prog_name)
{
    printf("\nUsage:\n");
    printf("\t%s [OPTION]\n", prog_name);
    printf("Options:\n");
    printf("\t[-t THREADS] Number of threads to use as our upper bound.\n");
    printf("\t[-u NUM] Number of upper bound for largest prime we desire.\n");
    printf("\t[-h] Print help information.\n");
}
