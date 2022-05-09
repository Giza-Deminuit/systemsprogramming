/*
Student name: Maggie Pettus
Student NetID: mnp223
Program Description: 
    This program generates a set of
    random numbers from three ranges and stores them in
    a binary file. The output file name, ranges, and number
    of values to generate have default values but may be
    specified by the user through the CLI.
*/

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

// union for converting int to 4-byte char buffer
union output
{
    int val;
    char bytes[4];
};

// Choose correct range based on number generated,
// add offset to chosen range's average, mult by 100,
// truncate to int.
int createVal(int lowVal, int midVal, int hiVal, int randNum, double offSet)
{
    int outVal;

    if (randNum <= 15)
        outVal = (lowVal + offSet) * 100;
    else if (randNum < 85)
        outVal = (midVal + offSet) * 100;
    else
        outVal = (hiVal + offSet) * 100;

    return outVal;
}

// *****************************************************************************
//                          M A I N    F U N C T I O N
// *****************************************************************************
int main(int argc, char *argv[])
{
     // seeding for random number generation
    srand(time(0));

    // Declare variables
    int outputFd, openFlags, i, randNum, valToStore, fd;
    double offSet;
    mode_t filePerms;
    union output out;

    // Set flags and permissions
    openFlags = O_CREAT | O_WRONLY | O_TRUNC;
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

    // Variables for arguments
    char *filename;
    int valToGen, lowVal, midVal, hiVal;

    // If no other arguments besides the program name were entered,
    // set the variables to their default values.
    if (argc == 1)
    {
        filename = "flowData.dat";
        valToGen = 60;
        lowVal = 5;
        midVal = 10;
        hiVal = 15;
    }
    // Else, if the number of values to generate
    // was specified by the user, assign the variables accordingly
    else if (argc == 3)
    {
        sscanf(argv[2], "%d", &valToGen);
        lowVal = 5;
        midVal = 10;
        hiVal = 15;
    }
    // Else, if the user supplies values for all arguments,
    // assign values accordingly
    else if (argc == 6)
    {
        sscanf(argv[2], "%d", &valToGen);
        sscanf(argv[3], "%d", &lowVal);
        sscanf(argv[4], "%d", &midVal);
        sscanf(argv[5], "%d", &hiVal);
    }
    // Else, the wrong number of arguments has been entered--abort
    else
    {
        perror("wrong number args");
        exit(EXIT_FAILURE);
    }

    // If the user specified a filename, open file with user-specified filename
    if (argc > 1)
    {
        fd = open(argv[1], openFlags, filePerms);
        if (fd == -1)
        {
            perror("open for write");
            exit(EXIT_FAILURE);
        }
    }
    // If the user did not specify a filename, open file with default filename
    else
    {
        fd = open(filename, openFlags, filePerms);
        if (fd == -1)
        {
            perror("open for write");
            exit(EXIT_FAILURE);
        }
    }
    

    // Loop while all numbers haven't been written to file
    for (i = 0; i < valToGen; i++)
    {
        // Generate a random number between 0 and 100
        randNum = rand() % (101);

        // Generate a random number between -.5 and .5
        offSet = (double)rand() / (double)RAND_MAX + (-.5);

        // Create value to be stored in file
        valToStore = createVal(lowVal, midVal, hiVal, randNum, offSet);

        // Optional - uncomment this line to have the values created display to screen
        // printf("%d\n", valToStore);

        // Use a union to convert valToStore to a 4 byte character buffer
        out.val = valToStore;

        // Write to file
        outputFd = write(fd, out.bytes, 2);
        if (outputFd == -1)
        {
            perror("Writing file");
            close(fd);
            exit(EXIT_FAILURE);
        }
    }

    close(fd);
    exit(EXIT_SUCCESS);
}