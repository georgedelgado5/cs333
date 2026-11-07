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
//          - Like who thought it was a good idea to create size_t AND ssize_t -> me no read good; me read one or two 's'? Too many double takes >!00!<
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
//#include <argon2.h>
#include "argon2.h"

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

// macros
#define DYNAMIC 'D'
#define DEFAULT_THREADS 1
#define BUF_SIZE 1000

//thread utilities
static int num_threads = DEFAULT_THREADS;
//static long index = 0;
//static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void help(char *);
static FILE *create_file(char *);
static int argon2(FILE *, FILE *);

//======================= Chaney source code w/mods ===========================
static char **create_line_array(char *, int);
static int count_lines(char *);
static char **create_ragged_array(char *, int *);
//======================= Chaney source code w/mods ===========================
    
//open flags
//static int open_flags = (O_CREAT | O_WRONLY | O_TRUNC);
//file permissions
static mode_t file_perms = (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
static ssize_t fst = 0;

int main(int argc, char *argv[])
{
    //get file size
    ssize_t hfst;
    ssize_t pfst;
    
    bool is_verbose = false;
    int opt = 0;
    char alloc_type = DYNAMIC;
    
    //file descriptors
    //int opfd = STDOUT_FILENO; //default is stdout

    FILE *opf = NULL;
    FILE *logf = NULL;

    //ragged arrays
    char **harray = NULL; //hash ragged array
    char **parray = NULL; //plain text ragged array
 
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
        int num_lines = 0;
        switch(opt)
        {
            case HASHES:
                harray = create_ragged_array(optarg, &num_lines);
                hfst = fst; 
                break;
            case PLAIN:
                parray = create_ragged_array(optarg, &num_lines);
                pfst = fst;
                break;
            case OUTPUT:
                if(optarg)
                {
                    //file created - no memory allocated
                    opf = create_file(optarg);
                }
                else
                {
                    opf = stdout;
                }
                break;
            case LOGFILE:
                if(optarg)
                {
                    //file created - no memory allocated
                    logf = create_file(optarg);
                }
                else
                {
                    logf = stderr;
                }
                break;
            case THREADS:
                if(optarg)
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

    if(num_threads < 0)
    {
        fprintf(logf, "Invalid value for number of threads %d\n", num_threads);
        fclose(logf);
        exit(EXIT_FAILURE);
    }

    //allocate memory for both output file and log file if they exist
    {
        ssize_t size = 0;
        if(hfst > pfst)
        {
            size = hfst;
        }
        size = pfst;

        //allocate memory for both output file and log file
        if(opf != stdout)
        {
            opf = malloc(size + 1);
        }
        if(logf != stderr)
        {
            logf = malloc(size + 1);
        }
    }

    //run static or dynamic threading
    {
        if(alloc_type == STATIC)
        {
            //do static allocation threading here
            argon2(opf, logf);
        }
        else
        {
            //do dynamic allocation threading here
            argon2(opf, logf);
        }
    }

    //close output file
    if(fclose(opf) == 0)
    {
        if(is_verbose)
        {
            printf("\nFiles successfuly closed\n");
        }
        else
        {
            if(is_verbose)
            {
                fprintf(logf, "\nFailed to close files\n");
            }
            exit(EXIT_FAILURE);
        }
    }

    //close log file
    if(fclose(logf) == 0)
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
    

    free(harray);
    free(parray);

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

// taking inspiration from Chaney create_ragged_array
static FILE *create_file(char *filename)
{
    char *ip_string = NULL;
    FILE *opf = NULL;
    
    ip_string = filename;
    opf = fopen(ip_string, "w");
    if(!opf)
    {
        fprintf(stderr, "\nFailed to open %s\n", ip_string);
        exit(EXIT_FAILURE);
    }
    //change file permissions
    if(chmod(ip_string, file_perms) == -1)
    {
        fprintf(opf, "\nFailed to set file permissions...");
        fclose(opf);
        exit(EXIT_FAILURE);
    }
    return opf;
}

//============================= Using section of Chaney source code from src/argon2/argon2_example.c ========================================
static int argon2(FILE *ipf, FILE *opf)
{
    char buf[BUF_SIZE] = {'\0'};
    char *hash = NULL;
    char *password = NULL;
    int result = 0;
    const argon2_type argon2_algo = Argon2_id;
    
    while(fgets(buf, BUF_SIZE, ipf) != NULL)
    {
        password = strtok(buf, ":\n");
        if(password == NULL)
        {
            fprintf(opf, "Invalid input >>%s<<\n", buf);
            continue;
        }
        hash = strtok(NULL, ":\n");

        //test print
        printf("TESTING: >>%s<< >>%s<<\n", password, hash);

        result = argon2_verify(hash, password, strlen(password), argon2_algo);
        if(result == ARGON2_OK)
        {
            fprintf(opf, "CRACKED: %s %s\n", password, hash);
            break;
        }
        else
        {
            fprintf(opf, "FAILED: %s\n", hash);
        }
    }
    return EXIT_SUCCESS;
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
    //set file size
    fst = file_stat.st_size;

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
