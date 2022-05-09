/*
	Student Name: Maggie Pettus
    Student NetID: mnp223
    Compiler Used: GCC
    Program Description:
    This program runs concurrently and uses System V shared memory and semaphores
    to randomly generate and print "modern ASCII art."
*/

// Problem 3
//#include "headers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/wait.h>
#include <time.h>
#include "binary_sem.h"
#include "semun.h"
#include <sys/shm.h>
#include <sys/sem.h>

#define C 0
#define P 1

#define MAX_BLOCKS 20

// Check for errors
int checkCall(int val, char *msg)
{
    if (val == -1)
    {
        if (errno == EINTR) return -1;
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return val;
}

// Problem 4 -- shared memory data structure
struct mSeg
{
    int numBlocks; // Number of blocks to generate
    int length[MAX_BLOCKS]; // Stores how many of each character
    char character[MAX_BLOCKS]; // Stores the characters to print
};

// Problem 5 -- CHILD FUNCTION
void childFunc(int semaphoreID, int sharedMemID)
{
    // a. Attach the segment of shared memory
    struct mSeg *atMem;
    atMem = (struct mSeg *) shmat(sharedMemID, NULL, 0);

    srand(time(NULL)); // b. Seed the random number generator
    reserveSem(semaphoreID, C); // c. Reserves the child semaphore

    // d. Generate and store the total number of blocks to generate.
    // ! This should be between 10 and 20
    int nBlocks = (rand() % 11) + 10;
    atMem->numBlocks = nBlocks;

    // c. For each block, generate and store the following values:
    for (int i = 0; i < nBlocks; i++)
    {
        // 1. The length of the block (between 2 and 10)
        int len = (rand() % 9) + 2;

        // 2. The character that comprises the block
        // ! This should be between 'a' and 'z' (97 to 122)
        int charNum = (rand() % 26) + 97;
        char charV = charNum; // Convert to a char betwen 'a' and 'z'

        // Store values
        atMem->length[i] = len;
        atMem->character[i] = charV;
    }

    releaseSem(semaphoreID, P); // d. Release the parent semaphore
    reserveSem(semaphoreID, C); // e. Reserve the child semaphore
    checkCall(shmdt(atMem), "shmdt"); // f. Detach the shared memory segment
    releaseSem(semaphoreID, P); // g. Release the parent semaphore

    exit(EXIT_SUCCESS);
}

// Problem 6 -- PARENT FUNCTION
void parentFunc(int semaphoreID, int sharedMemID)
{
    // a. Attach the segment of shared memory
    struct mSeg *atMem;
    atMem = (struct mSeg *) shmat(sharedMemID, NULL, 0);

    srand(time(NULL)); // b. Seeds the random number generator
    reserveSem(semaphoreID, P); // c. Reserve the parent semaphore

    // d. Generate a random width for the ASCII art
    //  ! This should between 10 and 15
    int width = (rand() % 6) + 10;

    // Output information about image
    printf("Width: %d\nBlocks: %d\nPairs: ", width, atMem->numBlocks);
    for (int a = 0; a < atMem->numBlocks; a++)
        printf("(%d, '%c')  ", atMem->length[a], atMem->character[a]);
    printf("\n\n");
    
    // e. Using the data stored in the shared memory segment, output an image
    int outSum = 0, j;

    for (int i = 0; i < atMem->numBlocks; i++)
    {
        for (j = 0; j < atMem->length[i]; j++)
        {
            printf("%c", atMem->character[i]);
            outSum++;
            if (outSum == width) { printf("\n"); outSum = 0; }
        }
    }
    printf("\n");

    releaseSem(semaphoreID, C); // f. Release the child semaphore
    reserveSem(semaphoreID, P); // g. Reserve the parent semaphore

    checkCall(shmdt(atMem), "shmdt"); // h. Detach the shared memory segment
}

int main(int argc, char *argv[])
{
    // 1. Create semaphore set of size 2 -- use IPC_PRIVATE as the key
    int semaphoreID = checkCall(semget(IPC_PRIVATE, 2, IPC_CREAT | S_IRUSR | S_IWUSR), "semget");
    
    initSemAvailable(semaphoreID, C); // a. Initalize the semaphore representing the child to available
    initSemInUse(semaphoreID, P); // b. Initalize the semaphore representing the parent to "in use"

    // 2. Create a segment of shared memory -- use IPC_PRIVATE as the key
    int sharedMemID = checkCall(shmget(IPC_PRIVATE, sizeof(struct mSeg), IPC_CREAT | S_IRUSR | S_IWUSR), "shmget");

    pid_t cPid = checkCall(fork(), "fork"); // 3. Create a child process
    if (cPid == 0) childFunc(semaphoreID, sharedMemID); // Send child to childFunc
    parentFunc(semaphoreID, sharedMemID); // 4. Send parent to parentFunc

    checkCall(semctl(semaphoreID, 0, IPC_RMID), "semctl"); // 5. Delete the semaphores
    checkCall(shmctl(sharedMemID, IPC_RMID, NULL), "shmctl"); // 6. Delete the shared memory

    checkCall(wait(NULL), "wait"); // Wait on child

    exit(EXIT_SUCCESS);
}