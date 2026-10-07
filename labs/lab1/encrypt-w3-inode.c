//George Delgado
//cs333
//decrypt-w3-inode
//
//Program will read input (either from a file named on the command line or 'stdin') and encrypt the file into the (required) named output file. 
//
//The encryption key (also the decryption key) are the low order three bytes from the 'inode' number of the output file.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>

#define INPUT 'i'
#define OUTPUT 'o'
#define HELP 'h'
#define VERBOSE 'v'

int main(int argc, char *argv[])
{
    //command line option
    char *cmd_line_opt = "i:o:vh";
    bool get_help = false;
    int opt = 0;

    //input/output file name
    char *ip_file_name = NULL; 
    char *op_file_name = NULL;
                
    //input default is stdin && output file (required)
    FILE *ip_file = stdin;
    FILE *op_file = NULL;

    //getopt
    while( (opt = getopt(argc, argv, cmd_line_opt) != -1) )
    {
        switch(opt)
        {
            case INPUT:
                //if no option is given, use fgets() to read in from stdin
                if(optarg != NULL)
                {
                    strcpy(ip_file_name, optarg);
                }
                break;
            case OUTPUT:
                if(optarg == NULL)
                {
                    break;
                }
                strcpy(op_file_name, optarg);
                break;
            case HELP:
                get_help = true;
                break;
            default:
                //do default thing
                break;
        }
    }
    
    if(argc < 1)
    {
        // add help support 
        printf("Invalid argument. Option must be provided.\n");
        printf("Did you mean %s [OPTION]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if(get_help)
    {
        printf("Description:");
        printf("\nReads input from named on the command line or stdin and encrypt the file into a (required) named output file.\n");
        printf("\nUsage:");
        printf("\n\t%s [-i] [OPTIONAL]\n", argv[0]);
        printf("\n\t%s [-o] [FILE]\n", argv[0]);
        printf("\nOptions:");
        printf("\n\t[-h] Print help information.\n");
        printf("\n\t[-v] Describe process being performed.\n");
        printf("\n\t[-i] Read in file name. If no file name is provided, stdin is used to read in data.\n");
        printf("\n\t[-o] Encrypts to output file. Requires file name option.\n");
        exit(EXIT_SUCCESS);
    }

    //from talk with Chaney
    //use read() and write() -> gives number of bytes
    //  -> may still need fgets for reading from stdin -> use whatever function can provide the data to a buffer and the size in bytes
    //  -> we only want to write the number of bytes we read
    //use stat() to get lower 3 bytes of inode for encryption/decryption key
    //  -> babbage uses little endian -> lower means LSB which is to the left (first) -> first 3 bytes
    //      -> says this is the hard part to get the 3 bytes 
    //**** see man 3 stat for the stucture of an inode ****
    //**** see man ? ? for data sizes in bytes/bits ****
    //
    // might not need [io]p_file[_name]
    
    return(EXIT_SUCCESS);
}
