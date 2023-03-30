/*
	Student Name: Maggie Pettus
    Student NetID: mnp223
    Compiler Used: GCC
    Program Description:
	This program forks a single child process which
    executes the "myRand" program.
    The data is read and displayed from the file dataX.dat,
    and the average of the values is computed and displayed.
*/

#include "headers.h"

#define BUF_SIZE 1024
static pid_t cPid;

// Check system calls for error
void checkCall(int val, const char *msg)
{
    if (val == -1)
    {
        perror(msg);
        exit(EXIT_FAILURE);
    }
}


int main(int argc, char *argv[])
{

    // Fork a child process
    cPid = fork();
    
    if (cPid == -1) // Error
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (cPid == 0)
    {
        // Run myRand
        char path[] = "./myRand";
        char *args[] = {"myRand", NULL};
        char *envVec[] = {NULL};

        checkCall(execve(path, args, envVec), "execve");
    }

    int stat, exitStatus;
    pid_t waitReturn;
    if ((waitReturn = wait(&stat)) == -1)
    {
        // If there are no child processes left, exit
        if (errno == ECHILD)
        {
            exit(EXIT_SUCCESS);
        }
        perror("waitpid");
        exit(EXIT_FAILURE);
    }

    // Convert satus to actual value and store it in exitStatus
    exitStatus = WEXITSTATUS(stat);

    // Open the file
    char buf[BUF_SIZE];
    sprintf(buf,"data%d.dat", exitStatus);
    
    int fd = open(buf, O_RDONLY);
    if (fd == -1)
    {
        perror("open for write");
        exit(EXIT_FAILURE);
    }

    // Read the values from the file
    int inBytes, val, i = 0;
    double sum = 0;

    while (inBytes = read(fd, &val, sizeof(int)) > 0)
    {
        i++;
        sum += val; // Sum all the values from file
        printf("Value %d: %d\n", i, val); // Display values
    }
    if (inBytes == -1)
    {
        perror("read");
        exit(EXIT_FAILURE);
    }

    // Compute and output the average of all values in the file
    //int average = sum/60;
    printf("\nAverage: %2.5f\n", (sum/60));
    checkCall(close(fd), "close");

    // Unlink the file
    sprintf(buf,"./data%d.dat", exitStatus);
    unlink(buf);

    exit(EXIT_SUCCESS);
}