/*George Delgado
 * gdelgado@pdx.edu
 * cs333
 * lab3
*/

/*
 *I used Chaney's src/ files as a resource. I also used AI to help me towards of the end of this assignment. 
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
//getpwuid, getgrgid -> user/group names for long toc
#include <pwd.h>
#include <grp.h>
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
#define BUF_SIZE 1024

bool is_magic(char *);
bool file_exists(char *);
void create(char *, char **);
char **get_members(int, char *[]);
void archive(int, char *);

void extract(char *);
void validate(char *);
void list_short(char *, bool);
void help(char *);

static bool is_verbose = false;
//open flags
static int open_flags = (O_CREAT | O_RDWR | O_TRUNC);
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
                case L_TABLE:
                    action = ACTION_TOC_LONG;
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

    if(!archive_file)
    {
        if(action != ACTION_CREATE)
        {
            archive_file = "stdin";
        }
        else
        {
            archive_file = "stdout";
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
            members = get_members(argc, argv); 
        }

        //check folio magic for all flags not including create
        //if does not contain folio magic, exit with bad magic
        if(action != ACTION_CREATE)
        {
            if(strcmp(archive_file, "stdin") == 0)
            {
                fprintf(stderr, "reading archive from stdin\n");
            }
            else if((action == ACTION_TOC_SHORT) || (action == (ACTION_TOC_LONG)))
            {
                fprintf(stderr, "reading archive file: \"%s\"\n", archive_file);
            }
            if(!is_magic(archive_file))
            {
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
                extract(archive_file);
                break;
            case ACTION_VALIDATE:
                validate(archive_file);
                break;
            case ACTION_TOC_SHORT:
                list_short(archive_file, false);
                break;
            case ACTION_TOC_LONG:
                list_short(archive_file, true);
                break;
            default:
                break;
        }
    }
    exit(EXIT_SUCCESS);
}

bool is_magic(char *file)
{
    int fd = STDIN_FILENO;
    //size of includes null terminator
    char buf[sizeof(FOLIO_MAGIC)] = {'\0'};

    if(strcmp(file, "stdin") != 0)
    {
        fd = open(file, O_RDONLY);
        if(fd == -1)
        {
            fprintf(stderr, "Failed to open file...\n");
            exit(CANNOT_OPEN_FOLIO);
        }
    }
    //using strlen here instead of sizeof to only read string bytes and not include null terminator
    if(-1 == read(fd, buf, strlen(FOLIO_MAGIC)))
    {
        fprintf(stderr, "Failed to read file...\n");
        if(strcmp(file, "stdin") !=0)
        {
            close(fd);
        }
        return false;
    }
    if(strcmp(file, "stdin") != 0)
    {
        close(fd);
    }
    if(strcmp(buf, FOLIO_MAGIC) != 0)
    {
        fprintf(stderr, "not a folio file: \"%s\"\n", file);
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
    int archive_fd = STDOUT_FILENO;

    if(strcmp(file, "stdout") != 0)
    {
        //open file storing old mask (rw-rw-r-- if new file)
        mode_t old_mask = umask(0);
        archive_fd = open(file, open_flags, file_perms);
        //restore old mask
        umask(old_mask);
        if(-1 == archive_fd)
        {
            fprintf(stderr, "Failed to open %s\n", file);
            exit(CANNOT_OPEN_FOLIO);
        }
    }
        
    //check if members (aka files) were provided
    if(!members)
    {
        if(-1 == write(archive_fd, FOLIO_MAGIC, strlen(FOLIO_MAGIC)))
        {
            fprintf(stderr, "\nFailed to write to %s\n", file);
            if(strcmp(file, "stdout") != 0)
            {
                close(archive_fd);
            }
            exit(EXIT_FAILURE);
        }
        if(strcmp(file, "stdout") != 0)
        {
            close(archive_fd);
        }
    }
    else
    {
        if(is_verbose)
        {
            fprintf(stderr, "These are the following members to be used:");
            for(int i = 0; members[i] != NULL; ++i)
            {
                if(i == 0)
                {
                    fprintf(stderr, "\n\t  %s", members[i]);
                }
                else
                {
                    fprintf(stderr, "\n\t, %s", members[i]);
                }

            }
            fprintf(stderr, "\n");
        }
        {
            //write folio magic to start with
            if(-1 == write(archive_fd, FOLIO_MAGIC, strlen(FOLIO_MAGIC)))
            {
                fprintf(stderr, "\nFailed to write to %s\n", file);
                if(strcmp(file, "stdout") != 0)
                {
                    close(archive_fd);
                }
                exit(EXIT_FAILURE);
            }
            //loop through members and create files
            for(int i = 0; members[i] != NULL; ++i)
            {
                archive(archive_fd, members[i]);
            }
            if(strcmp(file, "stdout") != 0)
            {
                close(archive_fd);
            }
        }
    }
}

//add one file at a time
void archive(int archive_fd, char *file)
{
    //here fd is each member (file) to be archived
    int fd = -1;
    folio_header_t header;
    folio_footer_t footer;
    struct stat member;
    ssize_t bytes_read = 0;

    //potential issues opening with permissions
    //might have to check permissions first
    fd = open(file, O_RDONLY);
    if(-1 == fd)
    {
        fprintf(stderr, "\nFailed to open %s", file);
        exit(CANNOT_OPEN_MEMB);
    }
    if(-1 == stat(file, &member))
    {
        fprintf(stderr, "\nFailed to stat %s", file);
        close(fd);
        exit(CANNOT_STAT_FILE);
    }

    //start setting data
    {
        memset(&header, 0, sizeof(header));
        memset(&footer, 0, sizeof(footer));
        header.st_size = member.st_size;
        header.st_mode = member.st_mode;
        header.st_uid = member.st_uid;
        header.st_gid = member.st_gid;
        header.st_atim = member.st_atim;
        header.st_mtim = member.st_mtim;
        header.name_length = strlen(file) + 1;
    }
            
    {
        MD5_CTX header_context;
       if(write(archive_fd, &header, sizeof(header)) != sizeof(header))
       {
           fprintf(stderr, "\nFailed to write %s", file);
           close(fd);
           exit(EXIT_FAILURE);
       }
       if(write(archive_fd, file, header.name_length) != (ssize_t)header.name_length) 
       {
           fprintf(stderr, "\nFailed to write %s", file);
           close(fd);
           exit(EXIT_FAILURE);
       }
        MD5Init(&header_context);
        MD5Update(&header_context, (const uint8_t *)&header, sizeof(header));
        MD5Update(&header_context, (const uint8_t *)file, header.name_length);
        MD5Final(footer.md5sum_header, &header_context);
    }

    //read and write data
    {
        unsigned char data_buf[BUF_SIZE] = {'\0'};
       MD5_CTX data_context;
       MD5Init(&data_context);
       while((bytes_read = read(fd, data_buf, BUF_SIZE)) > 0)
       {
           MD5Update(&data_context, data_buf, bytes_read);

            //typcast header.st_size because write returns ssize_t
           if(write(archive_fd, data_buf, bytes_read) != bytes_read)
           {
               fprintf(stderr, "\nFailed to write %s", file);
               close(fd);
               exit(EXIT_FAILURE);
           }
       }
       MD5Final(footer.md5sum_data, &data_context);
    }
    close(fd);

    //write footer
    {
       if(write(archive_fd, &footer, sizeof(footer)) != sizeof(footer))
       {
           fprintf(stderr, "\nFailed to write %s", file);
           exit(EXIT_FAILURE);
       }
    }
    if(is_verbose)
    {
        fprintf(stderr, "added: %s\n", file);
    }
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

void extract(char *file)
{
    int fd = STDIN_FILENO;
    folio_header_t header;
    folio_footer_t footer;
    unsigned char data_buf[BUF_SIZE] = {'\0'};

    if(strcmp(file, "stdin") != 0)
    {
        fd = open(file, O_RDONLY);
        if(-1 == fd)
        {
            fprintf(stderr, "Failed to open %s\n", file);
            exit(CANNOT_OPEN_FOLIO);
        }
        //skip past folio magic (already read for stdin)
        if(read(fd, data_buf, strlen(FOLIO_MAGIC)) != (ssize_t)strlen(FOLIO_MAGIC))
        {
            fprintf(stderr, "not a folio file\n");
            close(fd);
            exit(BAD_MAGIC);
        }
    }

    while(read(fd, &header, sizeof(header)) == (ssize_t)sizeof(header))
    {
        char *filename = NULL;
        int ofd = -1;
        off_t bytes_left = header.st_size;
        //times[0] -> atime, times[1] -> mtime
        struct timespec times[2];

        filename = malloc(sizeof(char) * header.name_length);
        if(!filename)
        {
            fprintf(stderr, "\nFailed to allocate memory");
            if(strcmp(file, "stdin") != 0)
            {
                close(fd);
            }
            exit(EXIT_FAILURE);
        }
        if(read(fd, filename, header.name_length) != (ssize_t)header.name_length)
        {
            fprintf(stderr, "\nFailed to read member name");
            free(filename);
            if(strcmp(file, "stdin") != 0)
            {
                close(fd);
            }
            exit(MEMBER_READ_FAIL);
        }

        //overwrite if file already exists
        ofd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, file_perms);
        if(-1 == ofd)
        {
            fprintf(stderr, "\nFailed to open %s", filename);
            free(filename);
            if(strcmp(file, "stdin") != 0)
            {
                close(fd);
            }
            exit(CANNOT_OPEN_MEMB);
        }

        //read and write data
        while(bytes_left > 0)
        {
            size_t bytes_to_read = BUF_SIZE;
            ssize_t bytes_read = 0;

            if(bytes_left < BUF_SIZE)
            {
                bytes_to_read = (size_t)bytes_left;
            }
            bytes_read = read(fd, data_buf, bytes_to_read);
            if(bytes_read <= 0)
            {
                fprintf(stderr, "\nFailed to read data for %s", filename);
                close(ofd);
                free(filename);
                if(strcmp(file, "stdin") != 0)
                {
                    close(fd);
                }
                exit(MEMBER_READ_FAIL);
            }
            if(write(ofd, data_buf, bytes_read) != bytes_read)
            {
                fprintf(stderr, "\nFailed to write %s", filename);
                close(ofd);
                free(filename);
                if(strcmp(file, "stdin") != 0)
                {
                    close(fd);
                }
                exit(EXIT_FAILURE);
            }
            bytes_left -= bytes_read;
        }

        //read footer to get to next header
        if(read(fd, &footer, sizeof(footer)) != (ssize_t)sizeof(footer))
        {
            fprintf(stderr, "\nFailed to read footer for %s", filename);
            close(ofd);
            free(filename);
            if(strcmp(file, "stdin") != 0)
            {
                close(fd);
            }
            exit(MEMBER_READ_FAIL);
        }

        //Chaney's order for timestamps (see lab appendix)
        times[0] = header.st_atim;
        times[1] = header.st_mtim;
        fchmod(ofd, header.st_mode);
        fsync(ofd);
        close(ofd);
        sync();
        if(-1 == utimensat(AT_FDCWD, filename, times, 0))
        {
            fprintf(stderr, "\nFailed to set timestamps on %s", filename);
        }

        if(is_verbose)
        {
            fprintf(stderr, "extracted: %s\n", filename);
        }
        free(filename);
    }
    if(strcmp(file, "stdin") != 0)
    {
        close(fd);
    }
}

void validate(char *file)
{
 int fd = STDIN_FILENO;
    folio_header_t header;
    folio_footer_t footer;
    unsigned char data_buf[BUF_SIZE] = {'\0'};
    int member_num = 0;

    if(strcmp(file, "stdin") != 0)
    {
        fd = open(file, O_RDONLY);
        if(-1 == fd)
        {
            fprintf(stderr, "Failed to open %s\n", file);
            exit(CANNOT_OPEN_FOLIO);
        }
        //skip past folio magic (already read for stdin)
        if(read(fd, data_buf, strlen(FOLIO_MAGIC)) != (ssize_t)strlen(FOLIO_MAGIC))
        {
            fprintf(stderr, "not a folio file\n");
            close(fd);
            exit(BAD_MAGIC);
        }
    }

    while(read(fd, &header, sizeof(header)) == (ssize_t)sizeof(header))
    {
        char *filename = NULL;
        off_t bytes_left = header.st_size;
        MD5_CTX header_context;
        MD5_CTX data_context;
        uint8_t md5_header[MD5_DIGEST_LENGTH] = {0};
        uint8_t md5_data[MD5_DIGEST_LENGTH] = {0};
        bool is_valid = true;

        filename = malloc(sizeof(char) * header.name_length);
        if(!filename)
        {
            fprintf(stderr, "\nFailed to allocate memory");
            if(strcmp(file, "stdin") != 0)
            {
                close(fd);
            }
            exit(EXIT_FAILURE);
        }
        if(read(fd, filename, header.name_length) != (ssize_t)header.name_length)
        {
            fprintf(stderr, "\nFailed to read member name");
            free(filename);
            if(strcmp(file, "stdin") != 0)
            {
                close(fd);
            }
            exit(MEMBER_READ_FAIL);
        }

        //header md5 includes the name, same as archive()
        MD5Init(&header_context);
        MD5Update(&header_context, (const uint8_t *)&header, sizeof(header));
        MD5Update(&header_context, (const uint8_t *)filename, header.name_length);
        MD5Final(md5_header, &header_context);

        //read data for data md5
        MD5Init(&data_context);
        while(bytes_left > 0)
        {
            size_t bytes_to_read = BUF_SIZE;
            ssize_t bytes_read = 0;

            if(bytes_left < BUF_SIZE)
            {
                bytes_to_read = (size_t)bytes_left;
            }
            bytes_read = read(fd, data_buf, bytes_to_read);
            if(bytes_read <= 0)
            {
                fprintf(stderr, "\nFailed to read data for %s", filename);
                free(filename);
                if(strcmp(file, "stdin") != 0)
                {
                    close(fd);
                }
                exit(MEMBER_READ_FAIL);
            }
            MD5Update(&data_context, data_buf, bytes_read);
            bytes_left -= bytes_read;
        }
        MD5Final(md5_data, &data_context);

        if(read(fd, &footer, sizeof(footer)) != (ssize_t)sizeof(footer))
        {
            fprintf(stderr, "\nFailed to read footer for %s", filename);
            free(filename);
            if(strcmp(file, "stdin") != 0)
            {
                close(fd);
            }
            exit(MEMBER_READ_FAIL);
        }

        //compare against md5s stored in footer
        //found -> what we computed, in file -> what the footer says
        ++member_num;
        printf("Validation for data member %d:\n", member_num);
        if(memcmp(md5_header, footer.md5sum_header, MD5_DIGEST_LENGTH) != 0)
        {
            is_valid = false;
            printf("\t*** Header MD5 does not match:\n");
        }
        else
        {
            printf("\tHeader MD5 does match:\n");
        }
        printf("\t\tfound:   ");
        for(int i = 0; i < MD5_DIGEST_LENGTH; ++i)
        {
            printf("%02x", md5_header[i]);
        }
        printf("\n\t\tin file: ");
        for(int i = 0; i < MD5_DIGEST_LENGTH; ++i)
        {
            printf("%02x", footer.md5sum_header[i]);
        }
        printf("\n");

        if(memcmp(md5_data, footer.md5sum_data, MD5_DIGEST_LENGTH) != 0)
        {
            is_valid = false;
            printf("\t*** Data MD5 does not match:\n");
        }
        else
        {
            printf("\tData MD5 does match:\n");
        }
        printf("\t\tfound:   ");
        for(int i = 0; i < MD5_DIGEST_LENGTH; ++i)
        {
            printf("%02x", md5_data[i]);
        }
        printf("\n\t\tin file: ");
        for(int i = 0; i < MD5_DIGEST_LENGTH; ++i)
        {
            printf("%02x", footer.md5sum_data[i]);
        }
        printf("\n");
        if(!is_valid)
        {
            printf("\t*** Validation failure:        %s for member %d\n", file, member_num);
        }
        free(filename);
    }
    if(strcmp(file, "stdin") != 0)
    {
        close(fd);
    }
}

void list_short(char *file, bool is_long)
{
  int fd = STDIN_FILENO;
    folio_header_t header;
    unsigned char buf[BUF_SIZE] = {'\0'};
    ssize_t bytes_read = 0;

    if(strcmp(file, "stdin") != 0)
    {
        fd = open(file, O_RDONLY);
        if(-1 == fd)
        {
            fprintf(stderr, "Failed to open %s\n", file);
            exit(CANNOT_OPEN_FOLIO);
        }
        //skip past folio magic (already read for stdin)
        if(read(fd, buf, strlen(FOLIO_MAGIC)) != (ssize_t)strlen(FOLIO_MAGIC))
        {
            fprintf(stderr, "not a folio file\n");
            close(fd);
            exit(BAD_MAGIC);
        }
    }

    printf("Contents of folio file: \"%s\"\n", file);

    while((bytes_read = read(fd, &header, sizeof(folio_header_t))) == (ssize_t)sizeof(folio_header_t))
    {
        char *filename = NULL;
        off_t bytes_to_skip = header.st_size;

        //[-T] needs the footer, so only skip data
        if(!is_long)
        {
            bytes_to_skip += (off_t)sizeof(folio_footer_t);
        }

        filename = malloc(sizeof(char) * header.name_length);
        if(!filename)
        {
            fprintf(stderr, "\nFailed to allocate memory");
            if(strcmp(file, "stdin") != 0)
            {
                close(fd);
            }
            exit(EXIT_FAILURE);
        }

        if(read(fd, filename, header.name_length) != (ssize_t)header.name_length)
        {
            fprintf(stderr, "\nFailed to read member name");
            free(filename);
            if(strcmp(file, "stdin") != 0)
            {
                close(fd);
            }
            exit(MEMBER_READ_FAIL);
        }

        printf("\tfile name: %s\n", filename);

        //lseek fails on stdin pipe -> read and toss instead
        if(lseek(fd, bytes_to_skip, SEEK_CUR) == -1)
        {
            while(bytes_to_skip > 0)
            {
                size_t bytes_to_read = BUF_SIZE;

                if(bytes_to_skip < BUF_SIZE)
                {
                    bytes_to_read = (size_t)bytes_to_skip;
                }
                bytes_read = read(fd, buf, bytes_to_read);
                if(bytes_read <= 0)
                {
                    fprintf(stderr, "\nFailed to skip member %s", filename);
                    free(filename);
                    if(strcmp(file, "stdin") != 0)
                    {
                        close(fd);
                    }
                    exit(MEMBER_READ_FAIL);
                }
                bytes_to_skip -= bytes_read;
            }
        }

        if(is_long)
        {
            folio_footer_t footer;
            char mode_buf[11] = "----------";
            char *perms = "rwxrwxrwx";
            char time_buf[64] = {'\0'};
            struct passwd *user = NULL;
            struct group *group = NULL;

            if(read(fd, &footer, sizeof(footer)) != (ssize_t)sizeof(footer))
            {
                fprintf(stderr, "\nFailed to read footer for %s", filename);
                free(filename);
                if(strcmp(file, "stdin") != 0)
                {
                    close(fd);
                }
                exit(MEMBER_READ_FAIL);
            }

            //build mode string (-rw-rw-r--)
            if(S_ISDIR(header.st_mode))
            {
                mode_buf[0] = 'd';
            }
            for(int i = 0; i < 9; ++i)
            {
                //S_IRUSR (0400) shifted down to S_IXOTH (0001)
                if(header.st_mode & (S_IRUSR >> i))
                {
                    mode_buf[i + 1] = perms[i];
                }
            }

            printf("\t\tmode:           %s\n", mode_buf);

            //print names, fall back to the id number if lookup fails
            user = getpwuid(header.st_uid);
            if(user)
            {
                printf("\t\tuser:           %s\n", user->pw_name);
            }
            else
            {
                printf("\t\tuser:           %d\n", (int)header.st_uid);
            }
            group = getgrgid(header.st_gid);
            if(group)
            {
                printf("\t\tgroup:          %s\n", group->gr_name);
            }
            else
            {
                printf("\t\tgroup:          %d\n", (int)header.st_gid);
            }

            printf("\t\tsize:           %ld\n", (long)header.st_size);
            //Chaney's time format -> 1965-07-08 02:10:35 PDT
            strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S %Z", localtime(&header.st_mtim.tv_sec));
            printf("\t\tmtime:          %s\n", time_buf);
            strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S %Z", localtime(&header.st_atim.tv_sec));
            printf("\t\tatime:          %s\n", time_buf);

            //print md5s as hex
            printf("\t\tmd5 sum header: ");
            for(int i = 0; i < MD5_DIGEST_LENGTH; ++i)
            {
                printf("%02x", footer.md5sum_header[i]);
            }
            printf("\n\t\tmd5 sum data:   ");
            for(int i = 0; i < MD5_DIGEST_LENGTH; ++i)
            {
                printf("%02x", footer.md5sum_data[i]);
            }
            printf("\n");
        }
        free(filename);
    }
    if(strcmp(file, "stdin") != 0)
    {
        close(fd);
    }
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
