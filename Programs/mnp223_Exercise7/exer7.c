/*
	Student Name: Maggie Pettus
    Student NetID: mnp223
    Compiler Used: GCC
    Program Description:
    This program demonstrates the use of a pipe
    to transfer user-entered string data from the parent
    to the child process.
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

// Child function
void childFunc(int fd[2])
{
    char inFromPipe[BUF_SIZE];
    int sizeofInput;

    checkCall(close(fd[1]), "close"); // Close write side of pipe

    checkCall(read(fd[0], &sizeofInput, sizeof(int)), "read"); // Read size of input
    checkCall(read(fd[0], inFromPipe, sizeofInput), "read"); // Read data from pipe

    // Output: "Child: <words from pipe>"
    checkCall(write(STDOUT_FILENO, "Child: ", strlen("Child: ")), "write");
    checkCall(write(STDOUT_FILENO, inFromPipe, sizeofInput), "write");
    printf("\n");
    
    checkCall(close(fd[0]), "close"); // Close read side of pipe

    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    int fd[2];
    pid_t cPid;
    checkCall(pipe(fd), "pipe"); // Set up pipe

    cPid = checkCall(fork(), "fork"); // Fork child
    if (cPid == 0) childFunc(fd); // Have the child run the childFunc function

    // ---------------------------------- Parent -----------------------------------------
    char input[BUF_SIZE];
    checkCall(close(fd[0]), "close"); // Close the read side of the pipe

    printf("Please enter text in the parent: "); // Prompt user

    scanf("%[^\n]s", input); // Get input from user
    int size = strlen(input); // Store length of input

    checkCall(write(fd[1], &size, sizeof(int)), "writing to pipe"); // Send size of string into child
    checkCall(write(fd[1], input, size), "writing to pipe"); // Send into pipe to child

    checkCall(wait(NULL), "wait"); // Wait on child

    exit(EXIT_SUCCESS);
}