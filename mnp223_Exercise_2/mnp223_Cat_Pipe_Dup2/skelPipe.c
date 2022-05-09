/*
	Student Name: Maggie Pettus
    Student NetID: mnp223
    Compiler Used: GCC
    Program Description:
    This program demonstrates using the dup2 function
    to allow a process to write and read character data to and from
    STDOUT. It also demonstrates execve and runs concurrently with
    a parent and child function.
*/

//#include "headers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>

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

    // If no file was specified by user, display error
    if (argc == 1) 
    {
        printf("You must enter the name of a file\n");
        exit(EXIT_SUCCESS);
    }

    int fd[2];
    pid_t cPid;
    checkCall(pipe(fd), "pipe"); // Set up pipe
    cPid = checkCall(fork(), "fork"); // Fork child
    
    // ---------------------------------- Child -----------------------------------------
    if (cPid == 0)
    {
        checkCall(close(fd[0]), "close"); // Close read side of pipe

        checkCall(dup2(fd[1], 1), "dup2"); // dup the write side to STDOUT

        // Execute the cat function on the file specified in the command line
        char command[] = "/usr/bin/cat";
        char *argV[] = {"cat", argv[1], NULL};
        char *envV[] = {NULL};
        if (execve(command, argV, envV) == -1)
        {
            perror("execve");
            exit(EXIT_FAILURE);
        }
        
        exit(EXIT_SUCCESS); // Just in case
    }

    // ---------------------------------- Parent -----------------------------------------
    checkCall(close(fd[1]), "close"); // Close write side of pipe

    // Variables for file reading
    char input[BUF_SIZE];
    ssize_t bytesFromInput;

    // While the pipe is not empty, continue reading
    while ((bytesFromInput = read(fd[0], input, BUF_SIZE-1)) != 0)
    {
        checkCall(bytesFromInput, "read");
        input[bytesFromInput] = '\0';
        checkCall(write(STDOUT_FILENO, input, strlen(input)), "write"); // Output to STDOUT
    }
    
    checkCall(wait(NULL), "wait"); // Wait on child

    exit(EXIT_SUCCESS);
}