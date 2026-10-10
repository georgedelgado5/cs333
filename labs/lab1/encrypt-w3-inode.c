//George Delgado
//cs333
//encrypt-w3-inode
//
//Program will read input (either from a file named on the command line or 'stdin') and encrypt the file into the (required) named output file. 
//
//The encryption key (also the decryption key) are the low order three bytes from the 'inode' number of the output file.
//
//stat(3) - for inode structure

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#define INPUT 'i'
#define OUTPUT 'o'
#define HELP 'h'
#define VERBOSE 'v'

int main(int argc, char *argv[])
{
    //command line option
    char *cmd_line_opt = "i:o:vh";
    bool get_help = false;
    bool is_verbose = false;
    int opt = 0; 
   
    //input/output file descriptors (fd) 
    //int ifd = 0;
    int ofd = 0; 
    
    //open flags
    int open_flags = (O_CREAT | O_WRONLY);
    //clear perms with umask
    mode_t default_perms = umask(0);
    //file permissions: -rw-rw-r--
    mode_t file_perms = (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH); 

    //input/output file name
    char *ip_file_name = NULL; 
    char *op_file_name = NULL;
                
    //getopt
    while((opt = getopt(argc, argv, cmd_line_opt)) != -1)
    {
        switch(opt)
        {
            case INPUT:
                ip_file_name = optarg;
                break;
            case OUTPUT:
                op_file_name = optarg;
                break;
            case HELP:
                get_help = true;
                break;
            case VERBOSE:
                is_verbose = true;
                break;
            default:
                break;
        }
    }

    if(get_help)
    {
        printf("Description:");
        printf("\nReads input from file name provided or stdin, if no file name is provided, and encrypts contents to an output file.\n");
        printf("\nUsage:\n");
        printf("\t%s [OPTION]\n", argv[0]);
        printf("Options:\n");
        printf("\t[-h] Print help information.\n");
        printf("\t[-v] Describe process being performed.\n");
        printf("\t[-i] [OPTIONAL] Read in file name. If no file name is provided, stdin is used to read in data.\n");
        printf("\t[-o] [REQUIRED] Encrypts to output file. Requires file name option.\n");
        exit(EXIT_SUCCESS);
    }
    
    //argv[0] is the name of the file and is argc at 1
    if(!op_file_name || argc == 1)
    {
        // add help support 
        fprintf(stderr, "Invalid argument. Option must be provided.\n");
        fprintf(stderr, "Try %s [-h] for help.\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    //open new (output) file
    {
        //add features for new file and existing file

        //flags are for creating new file; write only, and add permissions
        ofd = open(op_file_name, open_flags, file_perms);
        //reset perms
        umask(default_perms);
        if(ofd == -1)
        {
            fprintf(stderr, "\nFailed to open %s", op_file_name);
            exit(EXIT_FAILURE);
        }
        if(is_verbose)
        {
            printf("\nThe output file is %s", op_file_name); 
        }
    }

    //open input file or stdin
    if(!ip_file_name)
    {
        //read from stdin
        if(is_verbose)
        {
            printf("\nThe input file is STDIN");
        }
    }
    else
    {
        //read from ip_file_name
        if(is_verbose)
        {
            printf("\nThe input file is %s", ip_file_name);
        }
    }

    //read and XOR input file to new output file
    {

    }

    //steps to encrypt and write file
    //1. use stat() to get inode lower 3 values
    //2. store the lower 3 values to use as xor encryption
    //  a. example if inode id is 345808, the low 3 are 345
    //  b. use the bit version of 345 to encrypt each byte that is read in
    //3. read the input file byte by byte
    //  a. as each byte is read, xor with low 3 inode id values and write to output file
    //
    
    //from talk with Chaney
    //use read() and write() -> gives number of bytes
    //  -> may still need fgets for reading from stdin -> use whatever function can provide the data to a buffer and the size in bytes
    //  -> see week-02.day-02 lecture slides for standard I/O
    //  -> we only want to write the number of bytes we read
    //use stat() to get lower 3 bytes of inode for encryption/decryption key
    //  -> babbage uses little endian -> lower means LSB which is to the left (first) -> first 3 bytes
    //      -> says this is the hard part to get the 3 bytes 

    //close output file
    if(close(ofd) == 0)
    {
        if(is_verbose)
        {
            printf("\n%s successfuly closed", op_file_name);
        }
    }
    else
    {
        if(is_verbose)
        {
            fprintf(stderr, "\nFailed to close %s", op_file_name);
        }
        return(EXIT_FAILURE);
    }
    return(EXIT_SUCCESS);
}
