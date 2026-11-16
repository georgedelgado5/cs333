/*George Delgado
 * gdelgado@pdx.edu
 * cs333
 * lab3
*/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <fcntl.h>
//md5 -> see man 3bsd md5
#include <sys/types.h>
#include <md5.h>
//file descriptors STDOUT, STDIN, ...
#include <unistd.h>
//include Chaney's folio.h -> makefile using gcc -I option to include entire directory
#include "folio.h"

//getopt options
#define CREATE 'c'
#define EXTRACT 'x'
#define S_TABLE 't'
#define L_TABLE 'T'
#define VALIDATE 'V'
#define FILENAME 'f'
#define VERBOSE 'v'
#define HELP 'h'

//helper methods
void help(char *);

//static variables used across multiple methods
static char *archive_file = NULL;
static int folio_fd = -1; // used to read/write depending on extract/create
static bool is_verbose, to_validate, to_list, to_extract, to_create = false;
static struct stat folio_file;

int main(int argc, char *argv[])
{
    {
        int opt = 0;
        while((opt = getopt(argc, argv, OPTIONS)) != -1)
        {
            switch(opt)
            {
                case EXTRACT:
                    //if no [-f] [filename] option used, default to reading from stdin
                    to_extract = true;
                    break;
                case CREATE:
                    //if no [-f] [filename] option used, default to writing to stdout
                    to_create = true;
                    break;
                case S_TABLE:
                    to_list = true;
                    break;
                case FILENAME:
                    archive_file = optarg;
                    break;
                case VALIDATE:
                    to_validate = true;
                    break;
                case VERBOSE:
                    is_verbose = true;
                    break;
                case HELP:
                    help(argv[0]);
                    exit(NO_ACTION);
                default:
                    help(argv[0]);
                    perror("\nInvalid command line option");
                    fprintf(stderr, "Invalid command line option...\n");
                    exit(INVALID_CMD_OPTION);
                    break;
            }
        }
    }

    //check that multiple options weren't passed
    {    
        if(!to_extract && !to_create)
        {
            perror("\nOne option required: [-x] or [-c]");
            fprintf(stderr, "One option required: [-x] or [-c]...\n");
            exit(INVALID_CMD_OPTION);
        }
        if(to_extract && to_create)
        {
            perror("\nMultiple options provided: [-x] and [-c]");
            fprintf(stderr, "Multiple options provided: [-x] and [-c]...\n");
            exit(INVALID_CMD_OPTION);
        }
    }
    
    //process remaining command line arguments
    {
        if(optind < argc)
        {
            //IMPORTANT:
            // - if extracting, no command line arguments should be given
            //  - if verbose flag used, print members extracted
            // - if creating, process command line arguments
            //  - if verbose flags used, print members to add
            if(is_verbose)
            {
                fprintf(stdout, "\nThe following members will be used:\n");
            }
            //process remaining command line arguments...
            for(int i = optind; i < argc; ++i)
            {
                //what does processing the remaining arguments mean?
                //Do I need to add them to the folio_header_t member name
                if(is_verbose)
                {
                    printf("\t%s\n", argv[i]);
                }
            }
        }
    }

    //if folio file name passed, we can folio/pipe. Otherwise, use open(), read(), write()
    { 
        //open flags
        int open_flags = (O_CREAT | O_WRONLY | O_TRUNC);
        //file permissions: -rw-rw-r--
        mode_t file_perms = (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH); 

        if(!archive_file)
        {
            if(to_extract)
            {
                folio_fd = STDIN_FILENO;
                //add stream reading here
            }
            else 
            {
                folio_fd = STDOUT_FILENO;
                //add stream writing here
            }
        }
        else
        {
            //do not create file; only read
            if(to_extract)
            {
                folio_fd = open(archive_file, O_RDONLY);
                if(folio_fd == -1)
                {
                    perror("\nFailed to open file");
                    fprintf(stderr, "Failed to open...\n");
                    exit(CANNOT_OPEN_MEMB);
                }
            }
            else
            {
                folio_fd = open(archive_file, open_flags, file_perms);
                if(folio_fd == -1)
                {
                    perror("\nFailed to open file");
                    fprintf(stderr, "Failed to open...\n");
                    exit(CANNOT_OPEN_FOLIO);
                }
                //only if creating file
                //change file permissions
                if(fchmod(folio_fd, file_perms) == -1)
                {
                    perror("\nFailed to set file permissions");
                    fprintf(stderr, "\nFailed to set file permissions...");
                    close(folio_fd);
                    exit(CANNOT_OPEN_FOLIO);
                }
                //get stat info
                if(fstat(folio_fd, &folio_file) == -1)
                {
                    perror("\nFailed to stat file");
                    fprintf(stderr, "\nFailed to stat file...");
                    close(folio_fd);
                    exit(CANNOT_STAT_FILE);
                }
            }
        }
    }

    //validate magic number header first thing

    //fork and pipe here?


    if(is_verbose)
    {
        fprintf(stdout, "\nProviding additional information...\n");
    }
    return(EXIT_SUCCESS);
}

//add open(), read(), write() file handlers
/*
 * Binary files require using:
 * int open(),
 * int read(),
 * int write()
*/

//other utilities to consider
/*
 * lseek() -> move through folio sections,
 * fork(),
 * exec() family of functions
*/

//print help message
void help(char *prog_name)
{
    printf("\nDescription:");
    printf("\nAdd description.\n");
    printf("\nUsage:\n");
    printf("\t%s <OPTIONS> [-f FILENAME] [members [...]]\n", prog_name);
    printf("Options:\n");
    printf("\t[-x] Members extracted from folio file.\n");
    printf("\t[-c] Create a folio style archive file. Names of files to go into archive need to be provided.\n");
    printf("\t[-f] [FILENAME] Specify the name of the folio file on which to operate.\n");
    printf("\t[-V] Validate the MD5 value for the header and data for each archive member.\n");
    printf("\t[-t] List table of contents. Add '-v' for additional info.\n");
    printf("\t[-v] Provide additional information.\n");
    printf("\t[-h] Provide helpful information regarding program use.\n");
}
