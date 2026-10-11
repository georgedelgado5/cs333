//George Delgado
//cs333
//encrypt-w3-inode
//
//Program will read input (either from a file named on the command line or 'stdin') and encrypt the file into the (required) named output file. 
//
//The encryption key (also the decryption key) are the low order three bytes from the 'inode' number of the **output** file.
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
#define CLEAR_PERMS 0
#define MAX_BUFF_LEN 10000000
#define LOWER_THREE 3

int main(int argc, char *argv[])
{
    //command line option
    char *cmd_line_opt = "i:o:vh";
    bool get_help = false;
    bool is_verbose = false;
    int opt = 0; 
    unsigned char bytes[LOWER_THREE];
    
    //input/output file name
    char *ip_file_name = "STDIN"; //defaults 
    char *op_file_name = NULL;
   
    //input/output file descriptors (fd) 
    int ip_fd = STDIN_FILENO; //default to STDIN
    int op_fd = 0; 
    
    //open flags
    int open_flags = (O_CREAT | O_WRONLY);
    //clear perms with umask and store default perms
    mode_t default_perms = umask(CLEAR_PERMS);
    //file permissions: -rw-rw-r--
    mode_t file_perms = (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH); 

    //struct for input/output inode
    struct stat ip_file;
    struct stat op_file;

    //input/output bytes
    ssize_t ip_bytes_read = 0;
    ssize_t op_bytes_read = 0;
    char *ip_buff = NULL;
    char *op_buff = NULL;

    //encryption key
    unsigned long bit_mask = 0;
    unsigned long key = 0;
                
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
        fprintf(stderr, "Try %s [-h] for help.\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    //open new (output) file
    {
        op_fd = open(op_file_name, open_flags, file_perms);
        //reset perms back to default perms
        umask(default_perms);
        if(op_fd == -1)
        {
            fprintf(stderr, "\nFailed to open %s\n", op_file_name);
            exit(EXIT_FAILURE);
        }
        if(is_verbose)
        {
            printf("\nThe output file is %s", op_file_name); 
        }
        if(fstat(op_fd, &op_file) == -1)
        {
            fprintf(stderr, "\nFailed to access inode...");
        }
    }

    //open input file
    if(strcmp(ip_file_name, "STDIN") != 0)
    {
        ip_fd = open(ip_file_name, O_RDONLY);
        if(ip_fd == -1)
        {
            fprintf(stderr, "\nFailed to open %s\n", ip_file_name);
            exit(EXIT_FAILURE);
        }
        //read from ip_file_name
        if(is_verbose)
        {
            printf("\nSuccessfully opened %s...", ip_file_name);
        }
        if(fstat(ip_fd, &ip_file) == -1)
        {
            fprintf(stderr, "\nFailed to access inode...");
        }
        //dynamic allocation of memory to match size of input file -> get st_size
        ip_buff = malloc(ip_file.st_size + 1);
        op_buff = malloc(ip_file.st_size + 1);
    }

    //read from file
    {
        //read file/stdin
        if(ip_buff)
        {
            ip_bytes_read = read(ip_fd, ip_buff, ip_file.st_size);
        }
        else
        {
            ip_buff = malloc(MAX_BUFF_LEN + 1);
            ip_bytes_read = read(ip_fd, ip_buff, MAX_BUFF_LEN + 1);    
            op_buff = malloc(ip_bytes_read + 1);
        }
        if(ip_bytes_read == -1)
        {
            fprintf(stderr, "\nFailed to read from %s", ip_file_name);
            exit(EXIT_FAILURE);
        }
        if(is_verbose)
        {
            printf("\nReading from %s...", ip_file_name);
        } 
    }

    //xor input
    //from Chaney "sizeof" executable -> ino_t is 8 bytes/64 bits -> lower 3 bytes/24 bits -> other 5 bytes/40 bytes
    //unsigned long is 8 bytes -> leading bit is unsigned
    {
        //move a single bit to 24 bit and flip bits (from 0 -23) by subtracting 1
        bit_mask = (1ul << 24) - 1; 
        key = (op_file.st_ino & bit_mask);
        bytes[0] = (key & 0xFF);
        bytes[1] = (key >> 8) & 0xFF;
        bytes[2] = (key >> 16) & 0xFF;

        for(ssize_t i = 0; i < ip_bytes_read; i++)
        {
            //write to output file as xor every byte
            op_buff[i] = (ip_buff[i] ^ bytes[i % 3]);
        }
        op_bytes_read = write(op_fd, op_buff, ip_bytes_read);
        if(op_bytes_read == -1)
        {
            fprintf(stderr, "\nFailed to write to %s", op_file_name);
            exit(EXIT_FAILURE);
        }
        if(is_verbose)
        {
            printf("\nNumber of bytes written: %ld", op_bytes_read);
            printf("\nEncryption key is %lu", key);
        }
    }
    
    //close output file
    if(close(op_fd) == 0 && close(ip_fd) == 0)
    {
        if(is_verbose)
        {
            printf("\nFiles successfuly closed\n");
        }
    }
    else
    {
        if(is_verbose)
        {
            fprintf(stderr, "\nFailed to close files\n");
        }
        return(EXIT_FAILURE);
    }
    free(ip_buff);
    free(op_buff);
    return(EXIT_SUCCESS);
}
