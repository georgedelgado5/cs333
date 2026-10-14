//George Delgado
//cs333
//encrypt-v-symlink
//
//Program reads an input file or stdin, and encrypts the data into an output file.
//
//The encryption key (also the decryption key) is the name of the **symbolic link** pointing to the output file.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <ctype.h>

#define INPUT 'i'
#define OUTPUT 'o'
#define SYMBOLIC 's'
#define HELP 'h'
#define VERBOSE 'v'
#define OPTIONS "i:o:s:vh"
#define MAX_BUFF_LEN 10000000 //I don't know what size to default to...
#define NUM_BITS_8 8
#define ALPHABET_NUMS 26

void help(char *);
//Vigenere cipher
char encrypt(char, char);

int main(int argc, char *argv[])
{
    bool is_verbose = false;
    int opt = 0; 

    //input/output file name 
    char *ip_file_name = NULL;
    char *op_file_name = NULL;
    char *sl_file_name = NULL;
   
    //input/output file descriptors (fd) 
    int ip_fd = STDIN_FILENO; //default to STDIN
    int op_fd = 0; 

    //struct for input/output inode
    struct stat ip_file;
    struct stat op_file;

    //input bytes read
    ssize_t ip_bytes_rd = 0;

    //output bytes write
    ssize_t op_bytes_wr = 0;
    
    //input/output buffer
    unsigned char *ip_buff = NULL;
    unsigned char *op_buff = NULL;
                
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
            case SYMBOLIC:
                sl_file_name = optarg;
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

    if(!op_file_name || !sl_file_name)
    {
        fprintf(stderr, "\nNames for both [-o] and [-s] are required...");
        help(argv[0]);
        exit(EXIT_FAILURE);
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

        //set permissions
        if(fchmod(op_fd, file_perms) == -1)
        {
            fprintf(stderr, "\nFailed to set file permissions...");
        }
        
        //generate symbolic link to output file
        unlink(sl_file_name);
        {
            if(is_verbose)
            {
                fprintf(stderr, "\nUnlink old link...");
                exit(EXIT_FAILURE);
            }
        }
        if(symlink(op_file_name, sl_file_name) == -1)
        {
            fprintf(stderr, "\nFailed to create symbolic link...");
            unlink(sl_file_name);
            exit(EXIT_FAILURE);
        }
    }

    //encrypt file to op_buff
    {
        int j = 0;
        for(ssize_t i = 0; i < ip_bytes_rd; i++)
        {
            if(sl_file_name[j] == '\0')
            {
                j = 0;
            }
            op_buff[i] = encrypt(sl_file_name[j], ip_buff[i]);
            j++;
        }
    }

    //use encryption key to write to output file
    {
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

char encrypt(char key, char input)
{
    //wrap around 0 - 255 with unsigned
    unsigned char index = (unsigned char) input;
    unsigned char shift = (unsigned char) key;
    unsigned char encrypt = (unsigned char) (index + shift);
    return (char) encrypt;
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
    printf("\t[-s] [FILE] The name of the symbolic link to be created to point to the output file. (Required)\n");
    printf("\t[-h] Print help information.\n");
    printf("\t[-v] Describe process being performed.\n");
}
