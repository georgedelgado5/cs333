//George Delgado
//gdelgado@pdx.edu
//cs333
//lab2 - prince of aragon
//
//This program reads in two required input files, hash and plain text. The goal of this program is to identify if a password/key/code exists in the hashed and plain text file
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>
#include "argon2.h"
#include <time.h>

//getopt options
#define HASHES 'h'
#define PLAIN 'p'
#define OUTPUT 'o'
#define LOGFILE 'l'
#define THREADS 't'
#define STATIC 'S'
#define VERBOSE 'v'
#define HELP 'H'
#define OPTIONS "h:p:o:l:t:SvH"

// macros
#define DYNAMIC 'D'
#define DEFAULT_THREADS 1
#define NS_PER_SECOND 1000000000.0

static void *do_work(void *); //thread entry point -> like static void *find_primes(long) in tprimes 
static bool find_match(long); //called by thread entry
static long get_next_index(void); //like get next candidate from tprimes
static double elapse_time(struct timespec *, struct timespec *); //copied from tprimes -> calculates elapsed time
static FILE *create_file(char *);
static void help(char *);

//======================= Chaney source code w/mods ===========================
static char **create_line_array(char *, int);
static int count_lines(char *);
static char **create_ragged_array(char *, int *);
//======================= Chaney source code w/mods ===========================
    
static mode_t file_perms = (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
static int num_threads = 0; //number of threads -> defaults to '1' thread if no argument is passed
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static char alloc_type = DYNAMIC;
static FILE *op_file = NULL; //output file 
static FILE *log_file = NULL; //log file
static int hash_lines = 0; //number of lines in hash file
static int pwd_lines = 0; //number of lines in passwords file
static char **hash_array = NULL; //hash ragged array
static char **pwd_array = NULL; //password ragged array
static int cracked = 0;
static int failed = 0;

int main(int argc, char *argv[])
{
    struct timespec et0; 
    struct timespec et1;
    pthread_t *workers = NULL;
    double et = 0.0;
    bool is_verbose = false;
   
    //set default files
    num_threads = DEFAULT_THREADS;
    op_file = stdout;
    log_file = stderr;
    
    if(argc < 2)
    {
        fprintf(stderr, "%s [OPTION]... No options provided.\n", argv[0]);
        help(argv[0]);
        exit(EXIT_FAILURE);
    }

    //getopt
    {
        int opt = 0;
        while((opt = getopt(argc, argv, OPTIONS)) != -1)
        {
            switch(opt)
            {
                case HASHES:
                    hash_array = create_ragged_array(optarg, &hash_lines);
                    if(!hash_array)
                    {
                        fprintf(stderr, "\nFailed to create file...\n");
                        exit(EXIT_FAILURE);
                    }
                    break;
                case PLAIN:
                    pwd_array = create_ragged_array(optarg, &pwd_lines);
                    if(!pwd_array)
                    {
                        fprintf(stderr, "\nFailed to create file...\n");
                        exit(EXIT_FAILURE);
                    }
                    break;
                case OUTPUT:
                    //file created - no memory allocated
                    op_file = create_file(optarg);
                    break;
                case LOGFILE:
                    //file created - no memory allocated
                    log_file = create_file(optarg);
                    break;
                case THREADS:
                    {
                        int ret = sscanf(optarg, "%d", &num_threads);
                        if(ret != 1)
                        {
                            fprintf(stderr, "Invalid value for number of threads...\n");
                            exit(EXIT_FAILURE);
                        }
                    }
                    break;
                case STATIC:
                    alloc_type = STATIC;
                    break;
                case VERBOSE:
                    is_verbose = true;
                    if(argc < 3)
                    {
                        fprintf(stderr, "%s [-v] [OPTION] [...] must be provided...\n", argv[0]);
                        help(argv[0]);
                        exit(EXIT_FAILURE);
                    }
                    break;
                case HELP:
                default:
                    help(argv[0]);
                    exit(EXIT_SUCCESS);
                    break;
            }
        }
    }
    
    if(num_threads < 0)
    {
        fprintf(stderr, "\n%s [-t] [THREADS] requires a valid THREAD number...\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    clock_gettime(CLOCK_MONOTONIC, &et0);
    //allocate memory for threads aka workers
    workers = malloc(num_threads * sizeof(pthread_t));
    {
        //create threads -> "tid" = "thread index"
        for(long tid = 0; tid < num_threads; ++tid)
        {
            int ret = pthread_create(
                    &(workers[tid]) //thread(s)
                    , NULL
                    , do_work //method name
                    , (void *) tid //argument passed to method -> thread index
                    );
            if(ret != 0)
            {
                perror("Thread creation failed");
                fprintf(stderr, "\nThread creation failed on %ld\n", tid);
                exit(EXIT_FAILURE);
            }
        }
        
        //join threads
        for(int tid = 0; tid < num_threads; ++tid)
        {
            pthread_join(workers[tid], NULL);
        }

        //threads work completed and joined; free memory allocated for workers (thread array)
        free(workers);
        
    }
    clock_gettime(CLOCK_MONOTONIC, &et1);
    et = elapse_time(&et0, &et1);
    //fwrite(log_file);
    fprintf(log_file, "TOTAL: %d total: %d cracked: %d failed: %d et: %.4f\n", num_threads, (cracked+failed), cracked, failed, et);

    //close output file
    if(op_file != stdout)
    {
        fclose(op_file);
        if(is_verbose)
        {
            printf("\nFiles successfuly closed\n");
        }
    }

    //close log file
    if(log_file != stderr)
    {
        fclose(log_file);
        if(is_verbose)
        {
            printf("\nFiles successfuly closed\n");
        }
    }

    free(hash_array[0]);
    free(hash_array);
    free(pwd_array[0]);
    free(pwd_array);

    pthread_exit(NULL);
}

static void *do_work(void *arg)
{
    long tid = ((long) arg);
    long index = 0;
    struct timespec t0;
    struct timespec t1;
    double et = 0.0;
    int local_cracked = 0;
    int local_failed = 0;
    
    //record elapsed time t0
    clock_gettime(CLOCK_MONOTONIC, &t0);
    if(alloc_type == STATIC)
    {
        //index += num_threads is static allocation; we step by the number of threads 
        for(index = tid; index < hash_lines; index += num_threads)
        {
            if(find_match(index))
            {
                local_cracked++;
            }
            else
            {
                local_failed++;
            }
        }
    }
    else
    {
        index = get_next_index();
        while(index < hash_lines)
        {
            if(find_match(index))
            {
                local_cracked++;
            }
            else
            {
                local_failed++;
            }
            index = get_next_index();
        }
    }
    //record elapse time t1
    clock_gettime(CLOCK_MONOTONIC, &t1);
    //calculate total elapsed time (et)
    et = elapse_time(&t0, &t1);

    pthread_mutex_lock(&lock);
    //fwrite(op_file);
    fprintf(log_file, "THREAD: %ld total: %d cracked: %d failed: %d et: %.4f\n", tid, (local_cracked + local_failed), local_cracked, local_failed, et);
    pthread_mutex_unlock(&lock);

    cracked += local_cracked;
    failed += local_failed;

    pthread_exit(NULL);
}

static bool find_match(long index)
{
    int ret = 0;
    char *hash = hash_array[index];
    char *pwd = NULL;
    const argon2_type argon2_algo = Argon2_id; // const <datatype> <variable-name> = <argon2-defined-macro>;
    
    for(long line_index = 0; line_index < pwd_lines; line_index++)
    {
        pwd = pwd_array[line_index];
        ret = argon2_verify(hash, pwd, strlen(pwd), argon2_algo);
        if(ret == ARGON2_OK)
        {
            pthread_mutex_lock(&lock);
            fprintf(op_file, "CRACKED: %s %s\n", pwd, hash);
            pthread_mutex_unlock(&lock);
            return true;
        }
    }
    pthread_mutex_lock(&lock);
    fprintf(op_file, "FAILED: %s\n", hash);
    pthread_mutex_unlock(&lock);
    return false;
}

static long get_next_index(void)
{
    static long next_index = 0;
    long index = 0;

    pthread_mutex_lock(&lock);
    index = next_index;
    next_index++;
    pthread_mutex_unlock(&lock);

    return index;
}

//self explanatory
static double elapse_time(struct timespec *t0, struct timespec *t1)
{
    double et = (t1->tv_sec - t0->tv_sec)
        + ((t1->tv_nsec - t0->tv_nsec) / NS_PER_SECOND);
    return et;
}

//taking inspiration from Chaney create_ragged_array
//creates file for log or output if argument option is given
//no memory allocated
static FILE *create_file(char *filename)
{
    FILE *file = NULL;
    file = fopen(filename, "w"); //"w" has (O_WRONLY | O_CREAT | O_TRUNC) flags
    if(!file)
    {
        fprintf(stderr, "\nFailed to open %s\n", filename);
        exit(EXIT_FAILURE);
    }
    //change file permissions
    if(chmod(filename, file_perms) == -1)
    {
        fprintf(stderr, "\nFailed to set file permissions...");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    return file;
}

//print help message
//argument is program name -> argv[0]
static void help(char *prog_name)
{
    printf("\nDescription:");
    printf("\nMulti-threaded argon2 hash checker.\n");
    printf("\nUsage:\n");
    printf("\t%s [OPTION] [...]\n", prog_name);
    printf("Options:\n");
    printf("\t[-h] [FILENAME..] name of the input file of argon2 hashes.\n");
    printf("\t[-p] [FILENAME..] name of the input file of plaintext passwords.\n");
    printf("\t[-o] [FILENAME..] name of the output file. Defaults to stdout.\n");
    printf("\t[-l] [FILENAME..] the name of the log file. Defaults to stderr.\n");
    printf("\t[-t] [THREADS...] number of threads to use. Defaults to '1' thread.\n");
    printf("\t[-S] Run with static work allocation. Defaults to dynamic work allocation.\n");
    printf("\t[-v] Describe process being performed.\n");
    printf("\t[-H] Print help information.\n");
}

//================================================ Chaney source code w/modifications =================================================
//using Chaney ray-file src code
static char **create_ragged_array(char *filename, int *line_count)
{
    struct stat file_stat;
    char *ip_string = NULL;
	char **ra = NULL;
    int ipfd = -1;
    ssize_t bytes_read = 0;

    ipfd = open(filename, O_RDONLY);
    if (ipfd < 0) {
		perror("failed to open input file");
        fprintf(stderr, "\tfailed to open input file %s\n", filename);
        exit(EXIT_FAILURE);
    }
    if (fstat(ipfd, &file_stat) != 0) {
		perror("failed to stat input file");
        fprintf(stderr, "\tfailed to stat input file %s\n", filename);
        exit(EXIT_FAILURE);
    }
    
    ip_string = calloc(file_stat.st_size + 1, 1);
    bytes_read = read(ipfd, ip_string, file_stat.st_size);
    if (bytes_read != file_stat.st_size) {
        fprintf(stderr, "\tfailed to read input file %s\n", filename);
        exit(EXIT_FAILURE);
    }
	else {
		*line_count = count_lines(ip_string);
		ra = create_line_array(ip_string, *line_count);
	}

    close(ipfd);

    return ra;
}

//using Chaney ray-file src code
static int count_lines(char *string)
{
    int wc = 0;
    char *s = string;

	// This is rather lame. I could at least count both directions
	// at the same time to cut the time in half.
    for (int i = 0; *s ; ++i, s++) {
        if (*s == '\n') {
            wc++;
        }
    }

    return wc;
}

//using Chaney ray-file src code
static char **create_line_array(char *string, int wc)
{
    char **wa = malloc(wc * sizeof(char *));
    char *line = NULL;
    int index = 0;
	// strtok is not thread-safe
    line = strtok(string, "\n");
    while(line) {
        wa[index] = line;
		index++;
        line = strtok(NULL, "\n");
    }
    return wa;
}
