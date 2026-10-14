//George Delgado
//cs333
//encrypt-w3-inode
//
//Program reads an input file or stdin, and encrypts the data into an output file.
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
#define OPTIONS "i:o:vh"
#define MAX_BUFF_LEN 10000000 //I don't know what size to default to...
#define LOWER_THREE 3 //lower 3 bytes of inode id
#define NUM_BITS_8 8

void help(char *);

int main(int argc, char *argv[])
{
    bool is_verbose = false;
    int opt = 0; 

    //input/output file name 
    char *ip_file_name = NULL;
    char *op_file_name = NULL;
   
    //input/output file descriptors (fd) 
    int ip_fd = STDIN_FILENO; //default to STDIN
    int op_fd = 0; 

    //struct for input/output inode
    struct stat ip_file;
    struct stat op_file;

    //input bytes read
    ssize_t ip_bytes_rd = 0;
    
    //input/output buffer
    char *ip_buff = NULL;
    char *op_buff = NULL;
                
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
            case INPUT:
                ip_file_name = optarg;
                break;
            case OUTPUT:
                op_file_name = optarg;
                break;
            case HELP:
                help(argv[0]);
                exit(EXIT_SUCCESS);
                break;
            case VERBOSE:
                is_verbose = true;
                break;
            default:
                break;
        }
    }

    //open input file
    if(ip_file_name)
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
    
    //read input file/stdin
    {
        //from stdin
        if(ip_fd == STDIN_FILENO)
        {
            ip_buff = malloc(MAX_BUFF_LEN + 1);
            ip_bytes_rd = read(ip_fd, ip_buff, MAX_BUFF_LEN + 1); 
            op_buff = malloc(ip_bytes_rd + 1);
            ip_file_name = "stdin";
        }
        //from file input
        else
        {
            ip_bytes_rd = read(ip_fd, ip_buff, ip_file.st_size);
        }
        if(ip_bytes_rd == -1)
        {
            fprintf(stderr, "\nFailed to read from %s", ip_file_name);
            exit(EXIT_FAILURE);
        }
        if(is_verbose)
        {
            printf("\nReading from %s...", ip_file_name);
        } 
    }
    
    //open new (output) file
    {
        //open flags
        int open_flags = (O_CREAT | O_WRONLY | O_TRUNC);
        //file permissions: -rw-rw-r--
        mode_t file_perms = (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH); 
        
        //Man pages umask(2) state that a default set of permission are set when opening a file
            //The return value of umask is the previous permission settings -> stored so they can be recovered?
        //clear perms with umask and store default perms
        //const mode_t CLEAR_PERMS = 0; 
        //mode_t default_perms = umask(CLEAR_PERMS); //sets permissions to 0000 and stores previous perms -> now we can set to whatever we want

        op_fd = open(op_file_name, open_flags, file_perms);
        //reset perms back to default perms after opening and setting perms
        //umask(default_perms);
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

        /*
        // ******Or we can use fchmod which is easier to work -> umask was good practice but quite annoying to work with *******
        //set file permissions using fchmod after file has been created
        */
        if(fchmod(op_fd, file_perms) == -1)
        {
            fprintf(stderr, "\nFailed to set file permissions...");
        }
    }

    //xor input
    //from Chaney "sizeof" executable -> ino_t is 8 bytes/64 bits -> lower 3 bytes/24 bits -> little endian system
    {
        const unsigned char CHAR_BYTE = 0xFF; //a single byte (1's) that is used to XOR each of the lower 3 bytes
        unsigned long bit_mask = 0xFFFFFF; //sets the first 24 bits to 1's
        unsigned long key = (op_file.st_ino & bit_mask); //returns lower 3 bytes
        ssize_t op_bytes_wr = 0;

        //use sizeof or strlen to get the key length to replace the hardcoded '3'
        for(ssize_t i = 0; i < ip_bytes_rd; i++)
        {
            unsigned char encrypt_byte = (key >> (NUM_BITS_8 * (i % 3))) & CHAR_BYTE;
            //XOR to output buffer byte-by-byte
            op_buff[i] = (ip_buff[i] ^ encrypt_byte);
        }
            
        op_bytes_wr = write(op_fd, op_buff, ip_bytes_rd);
        if(op_bytes_wr == -1)
        {
            fprintf(stderr, "\nFailed to write to %s", op_file_name);
            exit(EXIT_FAILURE);
        }
        if(is_verbose)
        {
            printf("\nNumber of bytes written: %ld", op_bytes_wr);
        }
    }
    
    //close files
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

//print help message
//argument is program name -> argv[0]
void help(char *prog_name)
{
    printf("Description:");
    printf("\nReads an input file or stdin, and encrypts contents to an output file.\n");
    printf("\nUsage:\n");
    printf("\t%s [OPTION]\n", prog_name);
    printf("Options:\n");
    printf("\t[-i] [FILE] The name of the input file or stdin if no file name is provided.\n");
    printf("\t[-o] [FILE] The name of the output file. (Required)\n");
    printf("\t[-h] Print help information.\n");
    printf("\t[-v] Describe process being performed.\n");
}
