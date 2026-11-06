//George Delgado
//gdelgado@pdx.edu
//cs333
//lab2 - prince of aragon
//
//This program reads in two required input files, hash and plain text. The goal of this program is to identify if a password/key/code exists in the hashed and plain text file
//
//=========================== Citations and Resources ============================
//Resources and source code referenced include /u/rchaney/Classes/cs333/src/[argon2 | ray-file | threads]
//
//  My initial go of creating the ragged array was to open the file and iterate through each line with `getline(...)` count the number of lines. The count would indicate the number indices for the char ** array. Nothing new; I'm not reinventing the wheel;
//
//  Long story short, I end up doing my work later in the day and in a crunch that I don't feel like I'm "logicing" very well (personal problem). I want to be clear that I'm spending the time to think through the assignment albeit, a limited amount of time. Will probably bite me in the end (final exam...)
//
//  Therefore, I'm using chaney's src files as a direct resource. I'm including this because I want to be clear that I'm NOT presenting his work as my own!! Although I'm getting to the same logical conclusion (at a much slower rate...) that Chaney's code presents, I did NOT write it myself. I'm guessing that "src" code is available to use as a utility...
//      - The majority of "src" code that I'm utilizing is from ray-file to create the ragged array. 
//
//Additional Resources include:
//  - cs205 lecture slides that I saved -> reviewing ragged arrays
//  - Google searches include AI generated answers, stack overflow, and reddit...
//      - I'm sad to say that most of my google searches were for "dumb" searches such as data type byte size and basic examples of library calls
//      - As much as I like reading man pages (I've actually learned to appreciate them), I sometimes need help with example code (more on the syntax side)
//          - Like who though it was a good idea to create size_t AND ssize_t -> me no read good; me read one or two 's'? Too many double takes >!00!<
//              - Honestly, sometimes I'm just bad at logic. I guess it doesn't help to be doing this at 1am with only 5 hours of sleep...
//          - One example that was helpful was the man pages for getline(3)
//          - More often man pages doesn't have "good" examples... I need to get "gooder"... story of my life
//=========================== Citations and Resources ============================

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

//thread macros
#define DEFAULT_THREADS 1

//thread utilities
static int num_threads = DEFAULT_THREADS;
//static long index = 0;
//static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
// open flags
static int open_flags = (O_CREAT | O_WRONLY | O_TRUNC);
//file permissions
static mode_t file_perms = (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);

void help(char *);
static int create_file(char *);

//======================= Chaney source code w/mods ===========================
static char **create_line_array(char *, int);
static int count_lines(char *);
static char **create_ragged_array(char *, int *);
//======================= Chaney source code w/mods ===========================

int main(int argc, char *argv[])
{
    bool is_verbose = false;
    int opt = 0;
    char alloc_type = DYNAMIC;
    
    //file descriptors (fd)
    int op_fd = STDOUT_FILENO; //default is stdout
    int log_fd = STDERR_FILENO; //default is stderr

    int num_lines = 0;

    /*
    //file names
    char *op_file_name = NULL; 
    char *log_file_name = NULL;
    */

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
                rarray_arg2 = create_ragged_array(optarg, &num_lines);
                break;
            case PLAIN:
                rarray_ptxt = create_ragged_array(optarg, &num_lines);
                break;
            case OUTPUT:
                if(optarg)
                {
                    /*
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
                    */
                   op_fd = create_file(optarg);
                }
                else
                {
                    printf("\nNo argument provided with option [-o]. STDOUT will be used to write to...\n");
                }
                break;
            case LOGFILE:
                if(optarg)
                {
                    /*
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
                    */
                    log_fd = create_file(optarg);
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

    if(num_threads < 0)
    {
        dprintf(log_fd, "Invalid value for number of threads %d\n", num_threads);
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
                dprintf(log_fd, "\nFailed to close files\n");
            }
            exit(EXIT_FAILURE);
        }
    }

    free(rarray_arg2);
    free(rarray_ptxt);

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

static int create_file(char *filename)
{
    struct stat file_stat;
    char *ip_string = NULL;
    int ipfd = -1;
    
    ip_string = filename;
    ipfd = open(ip_string, open_flags, file_perms);
    if(ipfd < 0)
    {
        fprintf(stderr, "\nFailed to open %s\n", ip_string);
        exit(EXIT_FAILURE);
    }
    //change file permissions
    if(fchmod(ipfd, file_perms) == -1)
    {
        fprintf(stderr, "\nFailed to set file permissions...");
        close(ipfd);
        exit(EXIT_FAILURE);
    }
    //get file size
    if(fstat(ipfd, &file_stat) != 0)
    {
        fprintf(stderr, "\nFailed to access inode...");
        close(ipfd);
        exit(EXIT_FAILURE);
    }
    
    return ipfd;
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
