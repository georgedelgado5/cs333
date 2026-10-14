//George Delgado
//cs333
//decrypt-v-symlink
//
//Program reads a file pointed to by a symbolic link, and decrypts the data into an output file or stdout.
//
//The decryption key (also the encryption key) is the name of the **symbolic link** from the input file.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#define OUTPUT 'o'
#define SYMBOLIC 's'
#define HELP 'h'
#define VERBOSE 'v'
#define OPTIONS "o:s:vh"
//this is bad... baddddd... no good
//fix at a later time -> review how to get chunks of data using for or while loop
#define MAX_BUFF_LEN 10000000 //I don't know what size to default to...
#define NUM_BITS_8 8

void help(char *);
char decrypt(char, char);

int main(int argc, char *argv[])
{
    bool is_verbose = false;
    int opt = 0;
 
    //input/output file name
    char *ip_file_name = NULL; //name of the encrypted file the symbolic link points to
    char *op_file_name = NULL;
    char *sl_file_name = NULL;
 
    //length of the path stored in the symbolic link
    ssize_t link_len = 0;
 
    //input/output file descriptors (fd)
    int ip_fd = 0;
    int op_fd = STDOUT_FILENO; //default to STDOUT
 
    //struct for symbolic link/input inode
    struct stat sl_file;
    struct stat ip_file;
 
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
 
    //getopt
    while((opt = getopt(argc, argv, OPTIONS)) != -1)
    {
        switch(opt)
        {
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
 
    //only -s is required; output defaults to stdout
    if(!sl_file_name)
    {
        fprintf(stderr, "\nName for [-s] is required...\n");
        help(argv[0]);
        exit(EXIT_FAILURE);
    }
 
    //get the name of the file the symbolic link points to
    {
        //lstat looks at the link itself, not the file it points to
        if(lstat(sl_file_name, &sl_file) == -1)
        {
            fprintf(stderr, "\nFailed to access %s\n", sl_file_name);
            exit(EXIT_FAILURE);
        }
        if(!S_ISLNK(sl_file.st_mode))
        {
            fprintf(stderr, "\n%s is not a symbolic link\n", sl_file_name);
            exit(EXIT_FAILURE);
        }
 
        //dynamic allocation of memory to match size of link path -> get st_size
        //(for a symbolic link, lstat's st_size is the length of the path it points to)
        //+ 1 because readlink does not add a '\0'
        ip_file_name = malloc((size_t) sl_file.st_size + 1);
        link_len = readlink(sl_file_name, ip_file_name, (size_t) sl_file.st_size + 1);
        if(link_len == -1)
        {
            fprintf(stderr, "\nFailed to read symbolic link %s\n", sl_file_name);
            exit(EXIT_FAILURE);
        }
        ip_file_name[link_len] = '\0';
 
        if(is_verbose)
        {
            fprintf(stderr, "\n%s -> %s", sl_file_name, ip_file_name);
        }
    }
 
    //open input (encrypted) file
    {
        ip_fd = open(ip_file_name, O_RDONLY);
        if(ip_fd == -1)
        {
            fprintf(stderr, "\nFailed to open %s\n", ip_file_name);
            exit(EXIT_FAILURE);
        }
        if(is_verbose)
        {
            fprintf(stderr, "\nSuccessfully opened %s...", ip_file_name);
        }
        if(fstat(ip_fd, &ip_file) == -1)
        {
            fprintf(stderr, "\nFailed to access inode...");
            exit(EXIT_FAILURE);
        }
        //dynamic allocation of memory to match size of input file -> get st_size
        ip_buff = malloc((size_t) ip_file.st_size + 1);
        op_buff = malloc((size_t) ip_file.st_size + 1);
    }
 
    //read input file
    {
        ip_bytes_rd = read(ip_fd, ip_buff, (size_t) ip_file.st_size);
        if(ip_bytes_rd == -1)
        {
            fprintf(stderr, "\nFailed to read from %s", ip_file_name);
            exit(EXIT_FAILURE);
        }
        if(is_verbose)
        {
            fprintf(stderr, "\nReading from %s...", ip_file_name);
        }
    }
 
    //open new (output) file, only if -o was given; otherwise stdout
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
            fprintf(stderr, "\nThe output file is %s", op_file_name);
        }
 
        //set permissions
        if(fchmod(op_fd, file_perms) == -1)
        {
            fprintf(stderr, "\nFailed to set file permissions...");
        }
    }
 
    //decrypt file to op_buff
    {
        int j = 0;
        for(ssize_t i = 0; i < ip_bytes_rd; i++)
        {
            if(sl_file_name[j] == '\0')
            {
                j = 0;
            }
            op_buff[i] = (unsigned char) decrypt(sl_file_name[j], (char) ip_buff[i]);
            j++;
        }
    }
 
    //use decryption key to write to output file/stdout
    {
        op_bytes_wr = write(op_fd, op_buff, (size_t) ip_bytes_rd);
        if(op_bytes_wr == -1)
        {
            fprintf(stderr, "\nFailed to write to %s", op_file_name ? op_file_name : "stdout");
            exit(EXIT_FAILURE);
        }
        if(is_verbose)
        {
            fprintf(stderr, "\nNumber of bytes written: %zd", op_bytes_wr);
        }
    }
 
    free(ip_file_name);
    free(ip_buff);
    free(op_buff);
 
    //close files
    if(close(op_fd) == 0 && close(ip_fd) == 0)
    {
        if(is_verbose)
        {
            fprintf(stderr, "\nFiles successfuly closed\n");
        }
    }
    else
    {
        fprintf(stderr, "\nFailed to close files\n");
        return(EXIT_FAILURE);
    }


    return(EXIT_SUCCESS);
}

char decrypt(char key, char input)
{
    //reverse of encrypt: subtract the key byte; unsigned wraps around 0 - 255
    unsigned char index = (unsigned char) input;
    unsigned char shift = (unsigned char) key;
    unsigned char decrypt = (unsigned char) (index - shift);
    return (char) decrypt;
}
//print help message
//argument is program name -> argv[0]
void help(char *prog_name)
{
    printf("Description:");
    printf("\nReads an input file pointed to by a symbolic link, and decrypts contents to an output file or stdout.\n");
    printf("\nUsage:\n");
    printf("\t%s [OPTION]\n", prog_name);
    printf("Options:\n");
    printf("\t[-o] [FILE] The name of the output file.\n");
    printf("\t[-s] [FILE] The name of the symbolic link that points to the encrypted file. (Required)\n");
    printf("\t[-h] Print help information.\n");
    printf("\t[-v] Describe process being performed.\n");
}
