// EOPSY task 6
// author: Julia Czmut 300168

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define BUFFER_SIZE 2048

// FUNCTIONS

// function for copying the files using read() and write()
int copy_read_write(int fd_from, int fd_to) {

    // clear destination file
    if(ftruncate(fd_to, 0) == -1) {
        perror("copy_read_write(): Error while clearing destination file.\n");
        return 1;
    }
    
    // local variables for read() and write() - number of bytes read or written
    int bytes_read;  
    int bytes_written;

    char buffer[BUFFER_SIZE];    // buffer needed for read() and write() functions

    // copying the file using the buffer
    while((bytes_read = read(fd_from, buffer, BUFFER_SIZE)) > 0) {      // loop while there are still bytes to read - 0 would indicate end of the file

        bytes_written = write(fd_to, buffer, bytes_read);   // write the bytes which had been read from the source to the destination
        
        // checking for an error while write()
        if(bytes_written <= 0) {
            perror("copy_read_write(): Error while writing to the file.\n");
            return 1;
        }
    }
    return 0;
}

// function for copying the files using mmap()
int copy_mmap(int fd_from, int fd_to) {

    // getting source file status
    struct stat source_status;
    // checking for an error
    if(fstat(fd_from, &source_status) == -1) {    // fstat() returns info about a file; if 0 - success, if -1 - error
        perror("copy_mmap(): Error while getting source file status.\n");
        return 1;
    }

    // mapping source file to memory and obtaining a pointer pointing to it
    char* source_buff = mmap(NULL, source_status.st_size, PROT_READ, MAP_SHARED, fd_from, 0);   // pointer to the mapped area
    // checking for an error while mapping
    if(source_buff == (void*)-1) {      // in case of an error, MAP_FAILED value is returned
        perror("copy_mmap(): Error while mapping source file to memory.\n");
        return 1;
    }

    // adjusting the size of destination file to the size of source file and simultaneous check for an error
    if(ftruncate(fd_to, source_status.st_size) < 0) {   // successful ftruncate would return 0
        perror("copy_mmap(): Error while truncating destination file.\n");
        return 1;
    }

    // mapping destination file to memory and obtaining a pointer pointing to it
    char* destination_buff = mmap(NULL, source_status.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_to, 0);   // pointer to the mapped area
    // checking for an error while mapping
    if(destination_buff == (void*)-1) {
        perror("copy_mmap(): Error while mapping destination file to the memory.\n");
        return 1;
    }

    // copying the file and simultaneous check for an error
    if(memcpy(destination_buff, source_buff, source_status.st_size) == (void *)-1) {
        perror("copy_mmap(): Error while copying the file using memcpy().\n");
        return 1;
    }

    return 0;

}

// function for displaying 'help'
void help() {
    printf("COPY is a program for copying one file to another.\nThere are three usage options:\n\t./copy [-m] <file_name> <new_file_name>\n\t./copy <file_name> <new_file_name>\n\t./copy [-h]\n");
    printf("When -m flag is specified, the file is copied by mapping memory areas (mmap()) and copying (memcpy()).\nWhen no flag is chosen the file is copied using the read and write functions.\n When -h flag is specified, information about the program is printed.\n");
}


// ==========================================================================================================================================================================

int main(int argc, char* argv[]) {

    int opt = 0;            // variable for the result from getopt
    bool m_flag = false;    // value indicating presence of the -m flag

    // getting option arguments
    while((opt = getopt(argc, argv, ":hm")) != -1) {    // getopt returns -1 if there are no arguments left
        // ":hm" is the optstring argument of getopt() function; it contains the legitimate option characters; the semicolon before them means that getopt will not produce any diagnostic messages for missing option arguments or invalid options
        
        // case statement for the option arguments
        switch(opt) {
            case 'm':
                m_flag = true;
                break;
            case 'h':
                help();
                return 0;
            case '?':
                printf("Unknown option. See information below to see correct usage of the program.\n");
                help();
                return 1;
        }
    }

    // checking if the right number of arguments was specified
    if(argc <= 2 || (argc <= 3 && m_flag)) {       // too few arguments
        printf("Too few arguments provided.\n");
        help();
        return 1;
    }

    if((argc >= 4 && !m_flag) || (argc >= 5 && m_flag)) {       // too many arguments
        printf("Too many arguments provided.\n");
        help();
        return 1;
    }

    // if number of arguments is ok, obtaining file names from the arguments
    char* source_file_name;
    char* destination_file_name;

    if(m_flag) {    // if -m flag is used
        source_file_name = argv[2];
        destination_file_name = argv[3];
    } else {    // if no additional flag
        source_file_name = argv[1];
        destination_file_name = argv[2];
    }

    // initializing file descriptors
    int fd_from;    // file descriptor for the source file
    int fd_to;      // file descriptor for the destination file


    // _________________________________ OPENING FILES _________________________________



    // opening the source file
    fd_from = open(source_file_name, O_RDONLY);

    // checking for an error while opening
    if(fd_from < 0) {       // open() returns -1 on error
        perror("main(): Error while opening the source file.\n");
        return 1;
    }

    // obtaining source file status
    struct stat source_status;
    if(fstat(fd_from, &source_status) < 0) {   // fstat returns 0 on success, -1 on error
        perror("main(): Error while obtaining source file status.\n");
        return 1;
    }

    // opening the destination file
    fd_to = open(destination_file_name, O_RDWR | O_CREAT, source_status.st_mode);   // source file's status mode passed as mode parameter

    // checking for an error while opening
    if(fd_to < 0) {
        perror("main(): Error while opening the destination file.\n");
        return 1;
    }

    // _________________________________ COPYING FILES _________________________________

    int copy_result = 0;    // on success should be 0

    // copying in case of -m flag - mmap() and memcpy() are used
    if(m_flag) {
        copy_result = copy_mmap(fd_from, fd_to);
    }
    // copying in case of no additional flag - read() and write() are used
    else {
        copy_result = copy_read_write(fd_from, fd_to);
    }

    // _________________________________ CLOSING FILES _________________________________

    // closing the source file
    if(close(fd_from) < 0) {
        perror("main(): Error while closing the source file.\n");
        return 1;
    }

    // closing the destination file
    if(close(fd_to) < 0) {
        perror("main(): Error while closing the destination file.\n");
        return 1;
    }

    // end of the program, returning copy_result - 0 means success
    return copy_result;
}