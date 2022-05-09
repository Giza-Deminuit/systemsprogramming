/*
	Student Name: Maggie Pettus
    Student NetID: mnp223
    Compiler Used: GCC
    Program Description:
	This program creates a new file: dataX.dat where X
    is a randomly generated number from 0 - 255.
    60 random values between 0 and 100 are then generated and 
    written to the file.
    Exits with an exit status of X.
*/

#include "headers.h"

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


int main(int argc, char *argv[])
{
    srand(time(0)); // seeding for random number generation

    // Generate rand num between 0 and 255
    int randNum1, randNum2;
    randNum1 = rand() % (255);

    // Make new file: dataX.dat where X is a random number between 0 and 255
    char buf[BUF_SIZE];
    sprintf(buf,"data%d.dat", randNum1);
    int openFlags = O_CREAT | O_WRONLY | O_TRUNC;
    mode_t filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

    int fd = open(buf, openFlags, filePerms);
    if (fd == -1)
    {
        perror("open for write");
        exit(EXIT_FAILURE);
    }

    // Generate 60 values between 0 and 100 and store them in the file.
    for (int i = 0; i < 60; i++)
    {
        randNum2 = rand() % (100);
        checkCall(write(fd, &randNum2, sizeof(randNum2)), "write");
    }

    checkCall(close(fd), "close");

    // exit with and exit status of X
    exit(randNum1);
}