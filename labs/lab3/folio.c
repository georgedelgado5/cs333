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

//getopt options
#define EXTRACT 'x'
#define CREATE 'c'
#define S_TABLE 't'
#define L_TABLE 'T'
#define FILENAME 'f'
#define VALIDATE 'V'
#define VERBOSE 'v'
#define HELP 'h'
#define OPTIONS "xctTf:hvV"

//set default values -> use static if neccessary

//helper methods
void help(char *);
static void handle_file(char *);
static void create(void);
static void extract(void);
static void validate(void);
static void list_table(void);

//static variables used across multiple methods
static char *folio_file = NULL;
static int ip_file = -1;
static int op_file = -1;
//can I make one char array?
static char *members_c[] = {NULL}; //members to create
static char *members_x[] = {NULL}; //members to extract
static bool is_verbose = false;
static bool to_validate = false;
static bool to_list = false;

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
                    //get members_x
                    break;
                case CREATE:
                    //if no [-f] [filename] option used, default to writing to stdout
                    //get members_c
                    break;
                case S_TABLE:
                    to_list = true;
                    break;
                case FILENAME:
                    folio_file = optarg;
                    break;
                case VALIDATE:
                    to_validate = true;
                    break;
                case VERBOSE:
                    is_verbose = true;
                    break;
                case HELP:
                default:
                    help(argv[0]);
                    exit(EXIT_SUCCESS);
                    break;
            }
        }
    }

    if(!folio_file)
    {
        //do I have to pipe ip_file to op_file if both [-c] and [-x] are used with no [-f]
        //if(!members_c[0]) //create
        //{
        //  op_file = STDOUT_FILENO;
        //}
        //if(!members_x[0]) //extract
        //{
        //  ip_file = STDIN_FILENO;
        //}
    }
    
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

static void handle_file(char *filename)
{
    //should I pipe?
    ip_file = open(filename, O_RDONLY);
    if(ip_file == -1)
    {
        perror("Failed to open file");
        fprintf(stderr, "\nFailed to open %s...\n", filename);
        exit(EXIT_FAILURE);
    }

}

static void create(void)
{

}

static void extract(void)
{

}

static void validate(void)
{

}

static void list_table(void)
{
    /*
    if(is_verbose && to_list)
    {
        //print long table
        return;
    }
    //print short table
    */
}

//print help message
void help(char *prog_name)
{
    printf("\nDescription:");
    printf("\nAdd description.\n");
    printf("\nUsage:\n");
    printf("\t%s <OPTION> [...]\n", prog_name);
    printf("\t%s [-x] [member-1, ...]\n", prog_name);
    printf("\t%s [-c] [filename, ...]\n", prog_name);
    printf("Options:\n");
    printf("\t[-x] [...] Members extracted from folio file.\n");
    printf("\t[-c] [...] Create a folio style archive file. Names of files to go into archive need to be provided.\n");
    printf("\t[-f] [...] Specify the name of the folio file on which to operate.\n");
    printf("\t[-V] Validate the MD5 value for the header and data for each archive member.\n");
    printf("\t[-t] List table of contents. Add '-v' for additional info.\n");
    printf("\t[-v] Provide additional information.\n");
    printf("\t[-h] Provide helpful information regarding program use.\n");
}
