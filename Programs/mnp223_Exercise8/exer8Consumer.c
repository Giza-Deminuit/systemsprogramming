/*
	Student Name: Maggie Pettus
    Student NetID: mnp223
    Compiler Used: GCC
    Program Description:
    This program demonstrates the use of a FIFO
    to transfer user-entered string data from the parent
    to the child process.

    This is the consumer program.
*/

#include "headers.h"
#define BUF_SIZE 1024

int checkCall(int val, char *str)
{
    if (val == -1)
    {
        if (val == EINTR) return val;
        perror(str);
        exit(EXIT_FAILURE);
    }
    return val;
}

int main(int argc, char *argv[])
{
    /* attempt to make the FIFO */
    if (mkfifo("FIFOfile", S_IRUSR | S_IWUSR) == -1)
    {
        if (errno != EEXIST)
        {
            perror("mkfifo");
            exit(EXIT_FAILURE);
        } 
    }

    int fd = checkCall(open("FIFOfile", O_RDONLY), "open"); // Open FIFO for read

    char input[BUF_SIZE];
    int sizeofInput;

    checkCall(read(fd, &sizeofInput, sizeof(int)), "read"); // Read size of input
    checkCall(read(fd, input, sizeofInput), "read"); // Read data from FIFO

    // Output: "Consumer: <words from FIFO>"
    checkCall(write(STDOUT_FILENO, "Consumer: ", strlen("Consumer: ")), "write");
    checkCall(write(STDOUT_FILENO, input, sizeofInput), "write");
    printf("\n");

    checkCall(close(fd), "close"); // Close the FIFO

    exit(EXIT_SUCCESS);
}