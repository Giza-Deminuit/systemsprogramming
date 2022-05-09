/* Write a multiple concurrent process program that does the following
   1. Prompts the user for the number of integers to enter
   2. Prompts the user for each integer and writes them into 
      a file named data.dat
   3. Determines how many integers are > 100
   4. Determines how many integers are < 100
   5. Outputs the total count for each group.

   The program should perform this task in the following way:
   Create a producer child that:
   1. Prompts the user for the number of integers to enter
   2. Prompts the user for each integer and writes them into a file
      named data.dat
   Create a consumer child that:
1.	For each value in the file
a.	Determine if value > 100
b.	If the value > 100, signals the parent with SIGUSR1
   Create a consumer child that:
1.	For each value in the file
a.	Determines if value < 100
b.	If the value < 100, signals the parent with SIGUSR2
   Create a parent that: 
   1. Creates children 1 - 3
   2. Pauses. 

 */

/* Include files */
#include <stdio.h>
#include <stdlib.h>


/* Problem 1. Add the necessary include files for this program */
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>


/* 
   Global variables:
   For this project I am storing the pid's of the three children
   and two integers that serve the role of flags and counters
 */


/* Problem 2. Declare the global variables for pid's of the three
   children and the two integers that serve the role of flags and
   counters. The first flag deals with the large count; the second
   flag deals with the small count.
 */

static pid_t c1Pid;
static pid_t c2Pid;
static pid_t c3Pid;

static int large;
static int small;

#define BUF_SIZE 1024

// Check system calls for error
void checkCall(int val, const char *msg)
{
    if (val == -1)
    {
        perror(msg);
        exit(EXIT_FAILURE);
    }
}

/* myPrint is a convenience function to use when we are in a signal 
   handler. This is because printf uses buffered I/O.
*/
void myPrint(const char *str)
{
    if (write(STDOUT_FILENO, str, strlen(str)) == -1)
    {
        perror("write");
        exit(EXIT_FAILURE);
    }
}

/* Signal handlers
   We will need to implement at least two signal handlers. 
   One for the parent and a minimum of one for the children
*/

/* Problem 3. Define and write the signal handler for the parent */
static void handlePRNT(int sig)
{
    // Handle change of status in child process
    if (sig == SIGCHLD)
    {
        char other[1024] = { 0 };
        pid_t waitReturn = 0;
        int status;

        // Wait for child process
        while ((waitReturn = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED )) > 0)
        {
            if (WIFEXITED(status))
            {
                // If the child that is cleaned up is child 1,
                // send child 2 SIGUSR 1
                // send child 3 SIGUSR 2
                if (waitReturn == c1Pid)
                {
                    checkCall(kill(c2Pid,SIGUSR1),"kill");
                    checkCall(kill(c3Pid,SIGUSR2),"kill");
                }
            }
        }
        // If waitpid returned an error...
        if (waitReturn == -1)
        {
            // If there are no child processes left, output values and exit
            if (errno == ECHILD)
            {
                sprintf(other, "larger: %d\nsmaller: %d\n",large,small);
                checkCall(write(STDOUT_FILENO, other, strlen(other)),"write");

                exit(EXIT_SUCCESS);
            }
            perror("waitpid");
            exit(EXIT_FAILURE);
        }
    }

    if (sig == SIGUSR1) large++; // Handle child sending SIGUSR1 to parent
    if (sig == SIGUSR2) small++; // Handle child sending SIGUSR2 to parent
}

/* Problem 4. Define and write the signal handler for the children */
static void handleCHLD(int sig)
{
    if (sig == SIGUSR1) large = 1;
    if (sig == SIGUSR2) small = 1;
}

/* Functions for each of the children. 
   We will be writing functions for each of the three children.
   This should make it easier to answer the questions on threads.
*/

/* Problem 5. Define and write the function for child 1. */
void child1Func()
{
    int intstoenter, input, fd;
    char *filename = "data.dat";
    int openFlags = O_CREAT | O_WRONLY | O_TRUNC;
    mode_t filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

    printf("How many integers do you want to enter? ");
    scanf("%d", &intstoenter);

    fd = open(filename, openFlags, filePerms);
    if (fd == -1)
    {
        perror("open for write");
        exit(EXIT_FAILURE);
    }

    // Write numbers to file
    for (int i = 0; i < intstoenter; i++)
    {
        printf("Enter value %d: ",i+1);
        scanf("%d",&input);

        checkCall(write(fd, &input, sizeof(input)), "write");
    }

    checkCall(close(fd), "close"); // close file

    exit(EXIT_SUCCESS);
}


/* Problem 6. Define and write the function for child 2. */
void child2Func()
{
    // Assign sig handler for SIGUSR1
    struct sigaction sa_chld;
    checkCall(sigemptyset(&sa_chld.sa_mask),"sigemptyset");
    sa_chld.sa_handler = handleCHLD;
    sa_chld.sa_flags = 0;
    checkCall(sigaction(SIGUSR1, &sa_chld, NULL), "sigaction for SIGUSR1");

    // Loop while first flag (large) is 0
    while (large == 0) pause();

    // Reset the number of large ints to make count accurate
    large = 0;

    // open file data.dat for reading
    int fd = open("data.dat", O_RDONLY);
    if (fd == -1)
    {
        perror("Opening file for read");
        exit(EXIT_FAILURE);
    }

    int val, inBytes;

    while (inBytes = read(fd, &val, sizeof(int)) > 0)
    {
        if (val > 100)
        {
            large++;
            checkCall(kill(getppid(),SIGUSR1), "kill"); // Send signal to parent
        }
    }
    if (inBytes == -1)
    {
        perror("read");
        exit(EXIT_FAILURE);
    }

    checkCall(close(fd),"close");

    // Output total larger count
    char buf[BUF_SIZE];
    sprintf(buf,"total larger: %d\n", large);
    checkCall(write(STDOUT_FILENO, buf, strlen(buf)),"write");

    exit(EXIT_SUCCESS);
}

/* Problem 7. Define and write the function for child 3. */
void child3Func()
{
    // Assign sig handler for SIGUSR2
    struct sigaction sa_chld;
    checkCall(sigemptyset(&sa_chld.sa_mask),"sigemptyset");
    sa_chld.sa_handler = handleCHLD;
    sa_chld.sa_flags = 0;
    checkCall(sigaction(SIGUSR2, &sa_chld, NULL), "sigaction for SIGUSR2");

    // Loop while first flag (large) is 0
    while (small == 0) pause();

    // Reset the number of large ints to make count accurate
    small = 0;

    // open file data.dat for reading
    int fd = open("data.dat", O_RDONLY);
    if (fd == -1)
    {
        perror("Opening file for read");
        exit(EXIT_FAILURE);
    }

    int val, inBytes;

    while (inBytes = read(fd, &val, sizeof(int)) > 0)
    {
        if (val < 100)
        {
            small++;
            kill(getppid(),SIGUSR2); // Send signal to parent
        }
    }
    if (inBytes == -1)
    {
        perror("read");
        exit(EXIT_FAILURE);
    }

    checkCall(close(fd),"close");

    // Output total larger count
    char buf[BUF_SIZE];
    sprintf(buf,"total smaller: %d\n", small);
    checkCall(write(STDOUT_FILENO, buf, strlen(buf)),"write");

    exit(EXIT_SUCCESS);
}

/* This function forks a child and runs the function passed
   in after the child has successfully forked. I have provided
   it to make the code easier to read.
*/
pid_t hndlFork(void (*child)(void))
{
    pid_t p;
    p = fork();
    if (p == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (p == 0)
    {
        child();
    }
    return p;
}

/* Problem 8: Define and write the function main */
int main(int argc, char *argv[])
{
    // Assign signal handler for SIGCHLD
    struct sigaction sa_prnt;
    checkCall(sigemptyset(&sa_prnt.sa_mask),"sigemptyset");
    sa_prnt.sa_handler = handlePRNT;
    sa_prnt.sa_flags = 0;
    checkCall(sigaction(SIGCHLD, &sa_prnt, NULL), "sigaction for SIGCHLD");

    // Fork child 2 and child 3
    c2Pid = hndlFork(child2Func);
    c3Pid = hndlFork(child3Func);

    // Assign signal handler for SIGUSR1 and SIGUSR2
    checkCall(sigaction(SIGUSR1, &sa_prnt, NULL), "sigaction for SIGUSR1");
    checkCall(sigaction(SIGUSR2, &sa_prnt, NULL), "sigaction for SIGUSR2");

    // Fork child 1
    c1Pid = hndlFork(child1Func);

    while (1) pause();

  exit(EXIT_SUCCESS);
}
