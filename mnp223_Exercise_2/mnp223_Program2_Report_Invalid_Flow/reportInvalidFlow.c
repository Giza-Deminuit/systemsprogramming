/*
	Student Name: Maggie Pettus
    Student NetID: mnp223
    Compiler Used: GCC
    Program Description:
	This program opens a file, "flowData.dat", and reads
    its contents until the end of file has been reached.
    When pressure value is too low or too high, an alert
    will be printed to the screen, along with the number
    of values that have been read since the last alert.
*/

#include "includes.h"

// Union - used to convert two-byte data element into four-byte integer
union ISTR
{
    int val;
    char bytes[4];
};

// ****************************************************************************************************
//                                       M A I N    F U N C T I O N
// ****************************************************************************************************
int main(int argc, char *argv[])
{
    // Declare variables
    int i = 0;
    int numVals = 0;
    int totalValsRead = 0;
    int val;
    int fd;
    int numBytesRead; // used to store the number of bytes read from "flowData.dat"
    union ISTR in;
    int lowPressure = 550;
    int highPressure = 1450;

    // Open file with read-only permission
    fd = open("flowData.dat", O_RDONLY);

    // If the "open" command fails, inform user and abort
    if (fd == -1)
    {
        perror("Opening file for read");
        exit(EXIT_FAILURE);
    }

    in.val = 0; // Set union to 0

    // Start loop and read from file
    while (numBytesRead = read(fd, in.bytes, 2) != 0) // Read numbers from file until the end of file has been reached
    {
        // If the "read" command fails, inform user, close the file, and abort
        if (numBytesRead == -1)
        {
            perror("Reading file");
            close(fd);
            exit(EXIT_FAILURE);
        }

        // Optional code: uncomment line below to see all numbers as
        // they are read from the file
        // printf("%d\n", in.val); 

       // If the pressure is less than or equal to 550, print "Low Pressure" alert
       // along with how many values have been read since last alert
       if (in.val <= lowPressure)
       {
           printf("WARNING -- Pressure low: %d\n", in.val);
           printf("Number of values read since last alert: %d\n\n", numVals);
           numVals = 0; // Reset the number of values since last alert
       }
       // If the pressure is greater than or equal to 1450, print "High Pressure" alert
       // along with how many values have been read since last alert
       else if (in.val >= highPressure)
       {
          printf("WARNING -- Pressure high: %d\n", in.val);
          printf("Number of values read since last alert: %d\n\n", numVals);
          numVals = 0; // Reset the number of values since last alert
       }
       // Otherwise, number must be within acceptable range
       else
            numVals++; // Increment the number of values since last alert
        
        totalValsRead++;
        in.val = 0; // Set union to 0
    }

    // Optional code: uncomment line below to see 
    // how many numbers were read from the file in total
    // printf("Total values read: %d\n", totalValsRead);
    
    close(fd); // Close the file

    exit(EXIT_SUCCESS); // Exit program
}
