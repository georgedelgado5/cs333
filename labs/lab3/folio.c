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
//md5 -> see man 3 MD5Data
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

bool is_magic(char *);
bool file_exists(char *);
void create(char *, char **);
char **get_members(int, char *[]);
void archive(char *);

void extract(void);
void validate(void);
void list_short(void);
void list_long(void);
void help(char *);

//static variables used across multiple methods
static int archive_fd = -1; // used to read/write depending on extract/create
static bool is_verbose = false;
//open flags
static int open_flags = (O_CREAT | O_WRONLY | O_TRUNC);
//file permissions: -rw-rw-r--
static mode_t file_perms = (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH); 

int main(int argc, char *argv[])
{
    char *archive_file = NULL;
    char **members = NULL;
    folio_action_t action = ACTION_NONE;
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
            fprintf(stderr, "No options provided...\n");
            exit(INVALID_CMD_OPTION);
        }

        if((optind < argc) && (action != ACTION_CREATE))
        {
            fprintf(stderr, "Only create [-c] flag requires members...\n");
            exit(INVALID_CMD_OPTION);
        }
        
        if(optind < argc)
        {
            //get members here
            members = get_members(argc, argv); 
        }

        if(is_verbose && (action == ACTION_TOC_SHORT))
        {
            action = ACTION_TOC_LONG;
        }
        
        //check folio magic for all flags not including create
        //if does not contain folio magic, exit with bad magic
        if(action != ACTION_CREATE)
        {
            if(!is_magic(archive_file))
            {
                fprintf(stderr, "Invalid folio magic...\n");
                exit(BAD_MAGIC);
            }
        }
    }

    { 
        //use this as a "plan"; doesn't matter if you re-write code; start with something then make it better
        switch(action)
        {
            case ACTION_CREATE:
                create(archive_file, members);
                free(members);
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
    exit(EXIT_SUCCESS);
}

bool is_magic(char *file)
{
    //size of includes null terminator
    char buf[sizeof(FOLIO_MAGIC)] = {'\0'};
    int fd = -1;

    if(file)
    {
        fd = open(file, O_RDONLY);
        if(fd == -1)
        {
            fprintf(stderr, "Failed to open file...\n");
            return false;
        }
    }
    else
    {
        fd = STDIN_FILENO;
    }
    //using strlen here instead of sizeof to only read string bytes and not include null terminator
    if(-1 == read(fd, buf, strlen(FOLIO_MAGIC)))
    {
        fprintf(stderr, "Failed to read file...\n");
        if(file)
        {
            close(fd);
        }
        return false;
    }
    if(file)
    {
        close(fd);
    }
    if(strcmp(buf, FOLIO_MAGIC) != 0)
    {
        return false;
    }
    return true;
}

//is this necessary?
bool file_exists(char *file)
{
    struct stat file_stat;

    if(-1 == stat(file, &file_stat))
    {
        return false;
    }
    return true;
}

void create(char *file, char **members)
{
    if(file)
    {
        //open file
        archive_fd = open(file, open_flags, file_perms);
        if(-1 == archive_fd)
        {
            fprintf(stderr, "Failed to open %s\n", file);
            exit(CANNOT_OPEN_FOLIO);
        }
    }
    else
    {
        archive_fd = STDOUT_FILENO;
    }
        
    //check if members (aka files) were provided
    if(!members)
    {
        if(-1 == write(archive_fd, FOLIO_MAGIC, strlen(FOLIO_MAGIC)))
        {
            fprintf(stderr, "\nFailed to write to %s\n", file);
            if(file)
            {
                close(archive_fd);
            }
            exit(EXIT_FAILURE);
        }
        if(file)
        {
            close(archive_fd);
        }
    }
    else
    {
        if(is_verbose)
        {
            fprintf(stdout, "These are the following members to be used:");
            for(int i = 0; members[i] != NULL; ++i)
            {
                if(i == 0)
                {
                    fprintf(stdout, "\n\t  %s", members[i]);
                }
                else
                {
                    fprintf(stdout, "\n\t, %s", members[i]);
                }

            }
            fprintf(stdout, "\n");
        }
        {
            //write folio magic to start with
            if(-1 == write(archive_fd, FOLIO_MAGIC, strlen(FOLIO_MAGIC)))
            {
                fprintf(stderr, "\nFailed to write to %s\n", file);
                if(file)
                {
                    close(archive_fd);
                }
                exit(EXIT_FAILURE);
            }
            //loop through members and create files
            for(int i = 0; members[i] != NULL; ++i)
            {
                archive(members[i]);
            }
            close(archive_fd);
        }

    }
}

//add one file at a time
void archive(char *file)
{
    ssize_t bytes_write = 0;
    struct stat file_stat;
    folio_header_t header;
    //will add footer once I get to validation
    //folio_footer_t footer;
    int fd = -1;

    //potential issues opening with permissions
    //might have to check permissions first
    fd = open(file, O_RDONLY);
    if(-1 == fd)
    {
        fprintf(stderr, "\nFailed to open %s", file);
        exit(CANNOT_OPEN_MEMB);
    }
    if(-1 == stat(file, &file_stat))
    {
        fprintf(stderr, "\nFailed to stat %s", file);
        close(fd);
        exit(CANNOT_STAT_FILE);
    }

    //start setting data
    {
        header.st_size = file_stat.st_size;
        header.st_mode = file_stat.st_mode;
        header.st_uid = file_stat.st_uid;
        header.st_gid = file_stat.st_gid;
        header.st_atim = file_stat.st_atim;
        header.st_mtim = file_stat.st_mtim;
        header.name_length = strlen(file);
        strcpy(header.member_name, file);
    }

    //write header
    {
       bytes_write = write(archive_fd, &header, header.st_size); 
       if(-1 == bytes_write)
       {
           fprintf(stderr, "\nFailed to write %s", file);
           close(fd);
           exit(EXIT_FAILURE);
       }
    }

    bytes_write = 0;
    //write file name
    {
       bytes_write = write(archive_fd, &header.name_length, sizeof(header.name_length)); 
       if(-1 == bytes_write)
       {
           fprintf(stderr, "\nFailed to write %s", file);
           close(fd);
           exit(EXIT_FAILURE);
       }
    }
    close(fd);
}

//command line stuff
char **get_members(int argc, char *argv[])
{
    int member_count = (argc - optind);
    char **members = {NULL};
    int member_index = 0;

    if(!argv[optind])
    {
        return NULL;
    }
    //add one (+1) to get NULL terminator
    members = malloc((member_count + 1) * sizeof(members));
    if(!members)
    {
        fprintf(stderr, "\nMalloc failed...\n");
        exit(EXIT_FAILURE);
    }
    if(optind < argc)
    {
        for(int i = optind; argv[i] != NULL; ++i)
        {
            members[member_index] = argv[i];
            ++member_index;
        }
        //null terminate array of strings
        members[member_index] = NULL;
    }
    return members;
}

void extract(void)
{
         
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
