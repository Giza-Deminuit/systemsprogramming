/*
	Student Name: Maggie Pettus
    Student NetID: mnp223
    Compiler Used: GCC
    Program Description:
    This program demonstrates the use of a FIFO
    to transfer user-entered string data from the parent
    to the child process.

    This is the producer program.
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

    int fd = checkCall(open("FIFOfile", O_WRONLY), "open"); // Open FIFO

    char input[BUF_SIZE];

    printf("Please enter text in the producer: "); // Prompt user

    scanf("%[^\n]s", input); // Get input from user
    int size = strlen(input); // Store length of input

    checkCall(write(fd, &size, sizeof(int)), "writing to FIFO"); // Send size of string into FIFO
    checkCall(write(fd, input, size), "writing to FIFO"); // Send into FIFO to consumer

    checkCall(close(fd), "close"); // Close the FIFO

    exit(EXIT_SUCCESS);
}