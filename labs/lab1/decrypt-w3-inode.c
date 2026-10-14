//George Delgado
//cs333
//decrypt-w3-inode
//
//Program reads an input file, and decrypts the data into an output file or stdout.
//
//The decryption key (also the encryption key) are the low order three bytes from the 'inode' number of the **input** file.
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
    int ip_fd = 0;
    int op_fd = STDOUT_FILENO; //defaults to stdout
    
    //struct for input/output inode
    struct stat ip_file;
    struct stat op_file;

    //input bytes read
    ssize_t ip_bytes_rd = 0;
    
    //input/output buffer
    char *ip_buff = NULL;
    char *op_buff = NULL;
    
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
    {
        ip_fd = open(ip_file_name, O_RDONLY);
        if(ip_fd == -1)
        {
            fprintf(stderr, "\nFailed to open %s\n", ip_file_name);
            exit(EXIT_FAILURE);
        }
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

    //read input file
    {
        ip_bytes_rd = read(ip_fd, ip_buff, ip_file.st_size);
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
  
    //open output file
    if(op_file_name)
    {
        //open flags
        int open_flags = (O_CREAT | O_WRONLY | O_TRUNC);
        //file permissions: -rw-rw-r--
        mode_t file_perms = (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH); 
        
        op_fd = open(op_file_name, open_flags, file_perms);
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

        //set file permissions
        if(fchmod(op_fd, file_perms) == -1)
        {
            fprintf(stderr, "\nFailed to set file permissions...");
        }
    }
    else
    {
        op_file_name = "stdout";        
    }

    //xor input back to human readable code
    //from Chaney "sizeof" executable -> ino_t is 8 bytes/64 bits -> lower 3 bytes/24 bits -> little endian system
    {
        const unsigned char CHAR_BYTE = 0xFF; //a single byte (1's) that is used to XOR each of the lower 3 bytes
        unsigned long bit_mask = 0xFFFFFF; //sets the first 24 bits to 1's
        //====================================================================================================
        //====================================================================================================
        // IMPORTANT: encrypt and decrypt are nearly identical, with minor code rearrangements/modifications
        //      With the most important being -> the decrypt is the encrypt key from the input file
        unsigned long key = (ip_file.st_ino & bit_mask); //returns lower 3 bytes
        //====================================================================================================
        //====================================================================================================
        ssize_t op_bytes_wr = 0;

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
    printf("\nReads an encrypted input file, and decrypts contents to an output file or stdout.\n");
    printf("\nUsage:\n");
    printf("\t%s [OPTION]\n", prog_name);
    printf("Options:\n");
    printf("\t[-i] [FILE] The name of the input file. (Required)\n");
    printf("\t[-o] [FILE] The name of the output file.\n");
    printf("\t[-h] Print help information.\n");
    printf("\t[-v] Describe process being performed.\n");
}
