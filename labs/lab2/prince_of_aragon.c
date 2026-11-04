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

void help(char *);

int main(int argc, char *argv[])
{
    bool is_verbose = false;
    int opt = 0;
    int num_threads = DEFAULT_THREADS;
    char alloc_type = DYNAMIC;

    //buffer
    char *ip_buf_arg2 = NULL;
    char *ip_buf_ptxt = NULL;
    ssize_t ip_bytes_arg2 = 0;
    ssize_t ip_bytes_ptxt = 0;
                        
    // open flags
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
    struct stat log_stat;

    //file descriptors (fd)
    int ip_fd_arg2 = 0;
    int ip_fd_ptxt = 0;
    int op_fd = STDOUT_FILENO; //default is stdout
    int log_fd = STDERR_FILENO; //default is stderr
 
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
                    if(fstat(ip_fd_arg2, &ip_stat_arg2) == -1)
                    {
                        fprintf(stderr, "\nFailed to access inode...");
                    }
                    ip_buf_arg2 = malloc(ip_stat_arg2.st_size + 1);
                    ip_bytes_arg2 = read(ip_fd_arg2, ip_buf_arg2, ip_stat_arg2.st_size);
                    if(ip_bytes_arg2)
                    {
                        fprintf(stderr, "\nFailed to read in bytes...\n");
                        exit(EXIT_FAILURE);
                    }
                    if(close(ip_fd_arg2) == -1)
                    {
                        fprintf(stderr, "\nFailed to close hash file...\n");
                        exit(EXIT_FAILURE);
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
                    if(fstat(ip_fd_ptxt, &ip_stat_ptxt) == -1)
                    {
                        fprintf(stderr, "\nFailed to access inode...");
                    }
                    ip_buf_ptxt = malloc(ip_stat_ptxt.st_size + 1);
                    ip_bytes_ptxt = read(ip_fd_ptxt, ip_buf_ptxt, ip_stat_ptxt.st_size);
                    if(ip_bytes_ptxt)
                    {
                        fprintf(stderr, "\nFailed to read in bytes...\n");
                        exit(EXIT_FAILURE);
                    }
                    if(close(ip_fd_ptxt) == -1)
                    {
                        fprintf(stderr, "\nFailed to close file...\n");
                        exit(EXIT_FAILURE);
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
                    if(fstat(op_fd, &op_stat) == -1)
                    {
                        fprintf(stderr, "\nFailed to access inode...");
                    }
                    //add option to write to stdout or output file
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
                    if(fstat(log_fd, &log_stat) == -1)
                    {
                        fprintf(stderr, "\nFailed to access inode...");
                    }
                    //add option to write to stderr or log file
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

    //print variables to avoid make errors -> temporary
    printf("\nThe number of threads to use is %d\n", num_threads);

    //close files
    if(close(op_fd) == 0 && close(ip_fd_arg2) == 0 && close(ip_fd_ptxt) == 0 && close(log_fd) == 0)
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
    printf("\t[-h] Specify the name of the input file of argon2 hashes.\n");
    printf("\t[-p] Specify the name of the input file of plaintext passwords.\n");
    printf("\t[-o] Specify the name of the output file. Defaults to stdout.\n");
    printf("\t[-l] Specify the name of the log file. Defaults to stderr.\n");
    printf("\t[-t] Specify the number of threads to use. Defaults to '1' thread.\n");
    printf("\t[-S] Run with static work allocation. Defaults to dynamic work allocation.\n");
    printf("\t[-v] Describe process being performed.\n");
    printf("\t[-H] Print help information.\n");
}
