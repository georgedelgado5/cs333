//George Delgado
//gdelgado@pdx.edu
//cs333
//lab2 - prince of aragon

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>

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

//other macros
#define DYNAMIC 'D'
#define MAX_BUF_LEN 1000

// thread requirements
#define DEFAULT_THREADS 1
#define MAX_THREADS 16

void help(char *);

int main(int argc, char *argv[])
{
    bool is_verbose = false;
    int opt = 0;
    int num_threads = MAX_THREADS;
    char alloc_type = DYNAMIC;
                        
    int open_flags = (O_CREAT | O_WRONLY | O_TRUNC);
    //file permissions
    mode_t file_perms = (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);

    //file names
    char *ip_file_name_arg2 = NULL;
    char *ip_file_name_ptxt = NULL;
    char *op_file_name = NULL; 
    char *log_file_name = NULL;

    //structs
    struct stat ip_stat_arg2;
    struct stat ip_stat_ptxt;
    struct stat op_stat;

    //file descriptors (fd)
    int ip_fd_arg2 = 0;
    int ip_fd_ptxt = 0;
    int op_fd = STDOUT_FILENO;
    int log_fd = STDERR_FILENO;
 
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

    // getopt
    while((opt = getopt(argc, argv, OPTIONS)) != -1)
    {
        switch(opt)
        {
            case HASHES:
                if(optarg)
                {
                    ip_file_name_arg2 = optarg;
                    ip_fd_arg2 = open(ip_file_name_arg2, O_RDONLY);
                    if(ip_fd_arg2 == -1)
                    {
                        fprintf(stderr, "\nFailed to open %s\n", ip_file_name_arg2);
                        exit(EXIT_FAILURE);
                    }
                    if(is_verbose)
                    {
                        printf("\nSuccessfully opened %s...", ip_file_name_arg2);
                    }
                }
                break;
            case PLAIN:
                if(optarg)
                {
                    ip_file_name_ptxt = optarg;
                    ip_fd_ptxt = open(ip_file_name_ptxt, O_RDONLY);
                    if(ip_fd_ptxt == -1)
                    {
                        fprintf(stderr, "\nFailed to open %s\n", ip_file_name_ptxt);
                        exit(EXIT_FAILURE);
                    }
                    if(is_verbose)
                    {
                        printf("\nSuccessfully opened %s...", ip_file_name_ptxt);
                    }
                }
                break;
            case OUTPUT:
                if(optarg)
                {
                    op_file_name = optarg;
                    op_fd = open(op_file_name, open_flags, file_perms);
                    if(op_fd == -1)
                    {
                        fprintf(stderr, "\nFailed to open %s\n", op_file_name);
                        exit(EXIT_FAILURE);
                    }
                    if(fchmod(op_fd, file_perms) == -1)
                    {
                        fprintf(stderr, "\nFailed to set file permissions...");
                    }
                }
                break;
            case LOGFILE:
                if(optarg)
                {
                    log_file_name = optarg;
                    log_fd = open(log_file_name, open_flags, file_perms);
                    if(log_fd == -1)
                    {
                        fprintf(stderr, "\nFailed to open %s\n", log_file_name);
                        exit(EXIT_FAILURE);
                    }
                    if(fchmod(log_fd, file_perms) == -1)
                    {
                        fprintf(stderr, "\nFailed to set file permissions...");
                    }
                }
                break;
            case THREADS:
                if(!optarg)
                {
                    fprintf(stderr, "Number of threads is required...\n");
                    help(argv[0]);
                    exit(EXIT_FAILURE);
                }
                else
                {
                    num_threads = atoi(optarg);
                }
                break;
            case STATIC:
                alloc_type = STATIC;
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
    

    //close files
    if(close(op_fd) == 0 && close(ip_fd_arg2) == 0 && close(ip_fd_ptxt) == 0)
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
    printf("\t[-h] Specify the name of the input file of argon2 hashes.\n");
    printf("\t[-p] Specify the name of the input file of plaintext passwords.\n");
    printf("\t[-o] Specify the name of the output file. Defaults to stdout.\n");
    printf("\t[-l] Specify the name of the log file. Defaults to stderr.\n");
    printf("\t[-t] Specify the number of threads to use. Defaults to '1' thread.\n");
    printf("\t[-S] Run with static work allocation. Defaults to dynamic work allocation.\n");
    printf("\t[-v] Describe process being performed.\n");
    printf("\t[-H] Print help information.\n");
}
