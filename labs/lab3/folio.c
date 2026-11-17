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
void members(int, char *[]);
void create(void);
void extract(void);
void validate(void);
void list_short(void);
void list_long(void);
void help(char *);

//static variables used across multiple methods
static char *archive_file = NULL;
static int folio_fd = -1; // used to read/write depending on extract/create
static bool is_verbose, magic_mike = false;
static struct stat folio_file;
static folio_action_t action = ACTION_NONE;

int main(int argc, char *argv[])
{
    {
        int opt = 0;
        while((opt = getopt(argc, argv, OPTIONS)) != -1)
        {
            switch(opt)
            {
                case CREATE:
                    //if no [-f] [filename] option used, default to writing to stdout
                    action = ACTION_CREATE;
                    break;
                case EXTRACT:
                    //if no [-f] [filename] option used, default to reading from stdin
                    action = ACTION_EXTRACT;
                    break;
                case VALIDATE:
                    action = ACTION_VALIDATE;
                    break;
                case S_TABLE:
                    action = ACTION_TOC_SHORT;
                    break;
                case FILENAME:
                    archive_file = optarg;
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

    //handle cmd line critical
    {
        if(action == ACTION_NONE)
        {
            perror("\nNo options provided");
            fprintf(stderr, "No options provided...\n");
            exit(INVALID_CMD_OPTION);
        }
        
        if(is_verbose && (action == ACTION_TOC_SHORT))
        {
            action = ACTION_TOC_LONG;
        }
      
        if(optind < argc)
        {
            magic_mike = true;
        }
    }

    { 
        //use this as a "plan"; doesn't matter if you re-write code; start with something then make it better
        switch(action)
        {
            case ACTION_CREATE:
                create();
                break;
            case ACTION_EXTRACT:
                extract();
                break;
            case ACTION_VALIDATE:
                validate();
                break;
            case ACTION_TOC_SHORT:
                list_short();
                break;
            case ACTION_TOC_LONG:
                list_long();
                break;
            default:
                break;
        }
    }

    return(EXIT_SUCCESS);
}

//command line stuff
void members(int argc, char *argv[])
{
    struct stat member_stat;
    //folio_footer_t footer;
    //process remaining command line arguments
    if(optind < argc)
    {
        if(is_verbose)
        {
            fprintf(stdout, "\nThe following members will be used:\n");
        }
        for(int i = optind; i < argc; ++i)
        {
            //fstat members
            if(fstat(folio_fd, &member_stat) == -1)
            {
                perror("\nFailed to fstat file");
                fprintf(stderr, "\nFailed to fstat file...");
                close(folio_fd);
                exit(CANNOT_STAT_FILE);
            }
            //write members to stdout or file
            if(is_verbose)
            {
                printf("\t%s\n", argv[i]);
            }
        }
    }
}

void create(void)
{
    //open flags
    int open_flags = (O_CREAT | O_WRONLY | O_TRUNC);
    //file permissions: -rw-rw-r--
    mode_t file_perms = (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH); 

    //no archive-file provided
    if(!archive_file)
    {
        folio_fd = STDOUT_FILENO;
        //if no members provided
        if(!magic_mike)
        {
            if(-1 == write(folio_fd, FOLIO_MAGIC, strlen(FOLIO_MAGIC)))
            {
                perror("\nFailed to write magic");
                fprintf(stderr, "\nFailed to write magic...");
                exit(BAD_MAGIC);
            }
        }
        //members provided
        else
        {
            //write members to stdout
        }
    }
    //archive-file provided
    else
    {
        folio_fd = open(archive_file, open_flags, file_perms);
        if(folio_fd == -1)
        {
            perror("\nFailed to open file");
            fprintf(stderr, "Failed to open...\n");
            exit(CANNOT_OPEN_FOLIO);
        }
        //change file permissions
        if(fchmod(folio_fd, file_perms) == -1)
        {
            perror("\nFailed to set file permissions");
            fprintf(stderr, "\nFailed to set file permissions...");
            close(folio_fd);
            exit(CANNOT_OPEN_FOLIO);
        }
        //no members provided
        if(!magic_mike)
        {
            if(-1 == write(folio_fd, FOLIO_MAGIC, strlen(FOLIO_MAGIC)))
            {
                perror("\nFailed to write magic");
                fprintf(stderr, "\nFailed to write magic...");
                close(folio_fd);
                exit(BAD_MAGIC);
            }
        }
        //members provided
        else
        {
            //write members to file
        }
    }
}

void extract(void)
{
    if(!archive_file)
    {
        folio_fd = STDIN_FILENO;
        //add stream reading here
    }
    else
    {
        folio_fd = open(archive_file, O_RDONLY);
        if(folio_fd == -1)
        {
            perror("\nFailed to open file");
            fprintf(stderr, "Failed to open...\n");
            exit(CANNOT_OPEN_MEMB);
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

void validate(void)
{

}

void list_short(void)
{

}

void list_long(void)
{

}

void help(char *prog_name)
{
    printf("help text");
    printf("\n\tLab3/%s", prog_name);
    printf("\n\tOptions: %s", OPTIONS);
    printf("\n\t\t-x\t\textract file/files from archive");
    printf("\n\t\t-c\t\tcreate an archive file");
    printf("\n\t\t-t\t\tdisplay a short table of contents of the archive file");
    printf("\n\t\t-T\t\tdisplay a long table of contents of the archive file");
    printf("\n\t\tOnly one of xctTV can be specified");
    printf("\n\t\t-f filename	use filename as the archive file");
    printf("\n\t\t-V\t\tvalidate the MD5 hash values in the folio file");
    printf("\n\t\t-v\t\tgive verbose diagnostic messages");
    printf("\n\t\t-h\t\tdisplay this AMAZING help message");
    printf("\n");
}
