//George Delgado
//gdelgado@pdx.edu
//cs333
//lab2 - prince of aragon
//
//This program reads in two required input files, hash and plain text. The goal of this program is to identify if a password/key/code exists in the hashed and plain text file

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>

//getopt options
#define HASHES 'h'
#define PLAIN 'p'
#define OUTPUT 'o'
#define LOGFILE 'l'
#define THREADS 't'
#define STATIC 'S'
#define VERBOSE 'v'
#define HELP 'H'
#define OPTIONS "h:p:olt::SvH"

//other macros
#define DYNAMIC 'D'
#define MAX_BUF_LEN 1024 //standard byte buf size 2^10

//thread macros
#define DEFAULT_THREADS 1
#define MAX_THREADS 16

//thread utilities
static int num_threads = DEFAULT_THREADS;
//static long index = 0;
//static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void help(char *);

int main(int argc, char *argv[])
{
    bool is_verbose = false;
    int opt = 0;
    char alloc_type = DYNAMIC;

    //buffer
    char *ip_buf_arg2 = NULL;
    char *ip_buf_ptxt = NULL;

    //bytes
    ssize_t ip_bytes_arg2 = 0;
    ssize_t ip_bytes_ptxt = 0;
                        
    // open flags
    int open_flags = (O_CREAT | O_WRONLY | O_TRUNC);
    //file permissions
    mode_t file_perms = (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);

    //file names
    char *op_file_name = NULL; 
    char *log_file_name = NULL;

    //structs
    struct stat file_stat;

    //file descriptors (fd)
    int ip_fd = 0;
    int op_fd = STDOUT_FILENO; //default is stdout
    int log_fd = STDERR_FILENO; //default is stderr

    //ragged arrays
    char **rarray_arg2 = NULL;
    char **rarray_ptxt = NULL;
 
    if(argc < 2)
    {
        fprintf(stderr, "No options provided...\n");
        help(argv[0]);
        exit(EXIT_FAILURE);
    }
    if(argc < 3 && strcmp(argv[1], "-v") == 0)
    {
        fprintf(stderr, "File option must be provided...\n");
        help(argv[0]);
        exit(EXIT_FAILURE);
    }

    //getopt
    while((opt = getopt(argc, argv, OPTIONS)) != -1)
    {
        switch(opt)
        {
            case HASHES:
                //malloc a ragged array 
                {
                    //open file
                    ip_fd = open(optarg, O_RDONLY);
                    if(ip_fd == -1)
                    {
                        fprintf(stderr, "\nFailed to open %s\n", optarg);
                        exit(EXIT_FAILURE);
                    }
                    if(is_verbose)
                    {
                        printf("\nSuccessfully opened %s...", optarg);
                    }
                    //get file size
                    if(fstat(ip_fd, &file_stat) == -1)
                    {
                        fprintf(stderr, "\nFailed to access inode...");
                    }
                    ip_buf_arg2 = malloc(file_stat.st_size + 1);
                    ip_bytes_arg2 = read(ip_fd, ip_buf_arg2, file_stat.st_size);
                    if(ip_bytes_arg2)
                    {
                        fprintf(stderr, "\nFailed to read in bytes...\n");
                        exit(EXIT_FAILURE);
                    }
                    //close file
                    if(close(ip_fd) == -1)
                    {
                        fprintf(stderr, "\nFailed to close hash file...\n");
                        exit(EXIT_FAILURE);
                    }
                }
                break;
            case PLAIN:
                //malloc a ragged array 
                {
                    //open file
                    ip_fd = open(optarg, O_RDONLY);
                    if(ip_fd == -1)
                    {
                        fprintf(stderr, "\nFailed to open %s\n", optarg);
                        exit(EXIT_FAILURE);
                    }
                    if(is_verbose)
                    {
                        printf("\nSuccessfully opened %s...", optarg);
                    }
                    //get file size
                    if(fstat(ip_fd, &file_stat) == -1)
                    {
                        fprintf(stderr, "\nFailed to access inode...");
                    }
                    ip_buf_ptxt = malloc(file_stat.st_size + 1);
                    ip_bytes_ptxt = read(ip_fd, ip_buf_ptxt, file_stat.st_size);
                    if(ip_bytes_ptxt)
                    {
                        fprintf(stderr, "\nFailed to read in bytes...\n");
                        exit(EXIT_FAILURE);
                    }
                    //close file
                    if(close(ip_fd) == -1)
                    {
                        fprintf(stderr, "\nFailed to close file...\n");
                        exit(EXIT_FAILURE);
                    }
                }
                break;
            case OUTPUT:
                if(optarg)
                {
                    //open file
                    op_file_name = optarg;
                    op_fd = open(op_file_name, open_flags, file_perms);
                    if(op_fd == -1)
                    {
                        fprintf(stderr, "\nFailed to open %s\n", op_file_name);
                        exit(EXIT_FAILURE);
                    }
                    //change file permissions
                    if(fchmod(op_fd, file_perms) == -1)
                    {
                        fprintf(stderr, "\nFailed to set file permissions...");
                    }
                    //get file size
                    if(fstat(op_fd, &file_stat) == -1)
                    {
                        fprintf(stderr, "\nFailed to access inode...");
                    }
                    //add option to write to stdout or output file
                }
                else
                {
                    printf("\nNo argument provided with option [-o]. STDOUT will be used to write to...\n");
                }
                break;
            case LOGFILE:
                if(optarg)
                {
                    //open file
                    log_file_name = optarg;
                    log_fd = open(log_file_name, open_flags, file_perms);
                    if(log_fd == -1)
                    {
                        fprintf(stderr, "\nFailed to open %s\n", log_file_name);
                        exit(EXIT_FAILURE);
                    }
                    //change file permissions
                    if(fchmod(log_fd, file_perms) == -1)
                    {
                        fprintf(stderr, "\nFailed to set file permissions...");
                    }
                    //get file size
                    if(fstat(log_fd, &file_stat) == -1)
                    {
                        fprintf(stderr, "\nFailed to access inode...");
                    }
                    //add option to write to stderr or log file
                }
                else
                {
                    printf("\nNo argument provided with option [-l]. STDERR will be used to log...\n");
                }
                break;
            case THREADS:
                if(!optarg)
                {
                    fprintf(stderr, "Number of threads is required...\n");
                    help(argv[0]);
                    exit(EXIT_FAILURE);
                    //update to default to 1 thread
                }
                else
                {
                    num_threads = atoi(optarg);
                    if(is_verbose)
                    {
                        printf("\nNumber of threads to use is %d\n", num_threads);
                    }
                }
                break;
            case STATIC:
                alloc_type = STATIC;
                if(is_verbose)
                {
                    printf("\nAllocation type is static...\n");
                }
                //implement a static approach
                //  watch tprimes videos to review dynmic/static threads
                break;
            case VERBOSE:
                is_verbose = true;
                break;
            case HELP:
                help(argv[0]);
                exit(EXIT_SUCCESS);
                break;
            default:
                help(argv[0]);
                break;
        }
    }

    if((num_threads < 0) || (num_threads > MAX_THREADS))
    {
        fprintf(stderr, "Invalid value for number of threads %d\n", num_threads);
        exit(EXIT_FAILURE);
    }

    //from Chaney conversation... There are 3 major steps to this program
    //  1. complete getopt and malloc
    //  2. implement threads
    //  3. close out threads with join
    //
    //  At some point in between steps 1 and 2, a ragged array needs to be created

    if(alloc_type == STATIC)
    {
        //do something
    }
    else
    {
        //do something with DYNAMIC allocating
    }

    //close files
    if(close(op_fd) == 0 && close(log_fd) == 0)
    {
        if(is_verbose)
        {
            printf("\nFiles successfuly closed\n");
        }
        else
        {
            if(is_verbose)
            {
                fprintf(stderr, "\nFailed to close files\n");
            }
            exit(EXIT_FAILURE);
        }
    }

    free(ip_buf_arg2);
    free(ip_buf_ptxt);

    return(EXIT_SUCCESS);
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
    printf("\t[-h] (Required) name of the input file of argon2 hashes.\n");
    printf("\t[-p] (Required) name of the input file of plaintext passwords.\n");
    printf("\t[-o] Specify the name of the output file. Defaults to stdout.\n");
    printf("\t[-l] Specify the name of the log file. Defaults to stderr.\n");
    printf("\t[-t] Specify the number of threads to use. Defaults to '1' thread.\n");
    printf("\t[-S] Run with static work allocation. Defaults to dynamic work allocation.\n");
    printf("\t[-v] Describe process being performed.\n");
    printf("\t[-H] Print help information.\n");
}
