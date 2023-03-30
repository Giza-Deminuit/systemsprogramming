/*
	Student Name: Maggie Pettus
    Student NetID: mnp223
    Compiler Used: GCC
    Program Description:
    This program uses System V shared memory and semaphores
    to allow for a two-"player" game of tic tac toe.
    A FIFO is also used for synchronization.
    The game is played by basic algorithms that play optimally.
    As a result, the game will always tie.
*/

#include "headers.h"
#include "binary_sem.h"
#include <sys/shm.h>
#include <sys/sem.h>

#define P1 0
#define P2 1

int subOp; // Flag for sub-optimal play by player 2

/* define the struct that will represent the organization of our shared memory */
struct mSeg
{
    int turnCount;
    int arr[3][3];
};

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

// Displays the current state of the game board
void displayBoard(struct mSeg *atMem)
{
    if (atMem->turnCount == -1) printf("Final Results: \n");
    else printf("Turn: %d\n", atMem->turnCount + 1);

    printf(" ----------");
    for(int i = 0;i<3;i++)
    {
        printf("\n ");
        for(int j = 0;j < 3;j++)
        {
            switch(atMem->arr[i][j])
            {
                case 0: printf("| |"); break; // If 0, print empty
                case 1: printf("|X|"); break; // If 1, print X
                case -1: printf("|O|"); // If -1, print O
            }
        }
    }
    printf("\n ----------");
    printf("\n\n");
}

void randomPlace(int player, struct mSeg *atMem)
{
    int play, quitLoop = 0;
    if (player == 0) play = 1;
    else if (player == 1) play = -1;

    for(int i = 0;i < 3;i++)
    {
        for(int j=0;j<3;j++)
        {
            if (atMem->arr[i][j] == 0)
            {
                atMem->arr[i][j] = play;
                quitLoop = 1;
                break;
            }
        }
        if (quitLoop == 1) break; // If a move has been made, exit for loops
    }

    // If somehow, there are no free spaces and turnCount has not been updated to reflect this,
    // fix that here
    if (quitLoop == 0) atMem->turnCount = -1; 
}

int winSearch(int player, struct mSeg *atMem)
{
    // Change value of num based on which player called this function
    int two, one;
    if (player == 0) { two = 2; one = 1; }
    if (player == 1) { two = -2; one = -1; }

    // top row win
    if (atMem->arr[0][0] + atMem->arr[0][1] + atMem->arr[0][2] == two) 
        atMem->arr[0][0] = atMem->arr[0][1] = atMem->arr[0][2] = one;
    
    // mid row win
    else if (atMem->arr[1][0] + atMem->arr[1][1] + atMem->arr[1][2] == two) 
        atMem->arr[1][0] = atMem->arr[1][1] = atMem->arr[1][2] = one;

    // bottom row win
    else if (atMem->arr[2][0] + atMem->arr[2][1] + atMem->arr[2][2] == two) 
        atMem->arr[2][0] = atMem->arr[2][1] = atMem->arr[2][2] = one;
    
    // left column win
    else if (atMem->arr[0][0] + atMem->arr[1][0] + atMem->arr[2][0] == two) 
        atMem->arr[0][0] = atMem->arr[1][0] = atMem->arr[2][0] = one;
    
    // mid column win
    else if (atMem->arr[0][1] + atMem->arr[1][1] + atMem->arr[2][1] == two) 
        atMem->arr[0][1] = atMem->arr[1][1] = atMem->arr[2][1] = one;
    
    // right column win
    else if (atMem->arr[0][2] + atMem->arr[1][2] + atMem->arr[2][2] == two) 
        atMem->arr[0][2] = atMem->arr[1][2] = atMem->arr[2][2] = one;
    
    // down slope diagonal win
    else if (atMem->arr[0][0] + atMem->arr[1][1] + atMem->arr[2][2] == two) 
        atMem->arr[0][0] = atMem->arr[1][1] = atMem->arr[2][2] = one;
    
    // up slope diagonal win
    else if (atMem->arr[0][2] + atMem->arr[1][1] + atMem->arr[2][0] == two) 
        atMem->arr[0][2] = atMem->arr[1][1] = atMem->arr[2][0] = one;

    else return 0; // No win available

    return 1; // Player won
}

int blockSearch(int player, struct mSeg *atMem)
{
    // Change value of num based on which player called this function
    int two, one;
    if (player == 0) { two = -2; one = 1; }
    if (player == 1) { two = 2; one = -1; }

    if (atMem->arr[0][0] + atMem->arr[0][1] + atMem->arr[0][2] == two) // top row danger
    {
        // Find the empty square and put current player's mark in it to block
        if (atMem->arr[0][0] == 0) atMem->arr[0][0] = one;
        if (atMem->arr[0][1] == 0) atMem->arr[0][1] = one;
        if (atMem->arr[0][2] == 0) atMem->arr[0][2] = one;
    }
    else if (atMem->arr[1][0] + atMem->arr[1][1] + atMem->arr[1][2] == two) // mid row danger
    {
        if (atMem->arr[1][0] == 0) atMem->arr[1][0] = one;
        if (atMem->arr[1][1] == 0) atMem->arr[1][1] = one;
        if (atMem->arr[1][2] == 0) atMem->arr[1][2] = one;
    }
    else if (atMem->arr[2][0] + atMem->arr[2][1] + atMem->arr[2][2] == two) // bottom row danger
    {
        if (atMem->arr[2][0] == 0) atMem->arr[2][0] = one;
        if (atMem->arr[2][1] == 0) atMem->arr[2][1] = one;
        if (atMem->arr[2][2] == 0) atMem->arr[2][2] = one;
    }
    else if (atMem->arr[0][0] + atMem->arr[1][0] + atMem->arr[2][0] == two) // left column danger
    {
        if (atMem->arr[0][0] == 0) atMem->arr[0][0] = one;
        if (atMem->arr[1][0] == 0) atMem->arr[1][0] = one;
        if (atMem->arr[2][0] == 0) atMem->arr[2][0] = one;
    }
    else if (atMem->arr[0][1] + atMem->arr[1][1] + atMem->arr[2][1] == two) // mid column danger
    {
        if (atMem->arr[0][1] == 0) atMem->arr[0][1] = one;
        if (atMem->arr[1][1] == 0) atMem->arr[1][1] = one;
        if (atMem->arr[2][1] == 0) atMem->arr[2][1] = one;
    }
    else if (atMem->arr[0][2] + atMem->arr[1][2] + atMem->arr[2][2] == two) // right column danger
    {
        if (atMem->arr[0][2] == 0) atMem->arr[0][2] = one;
        if (atMem->arr[1][2] == 0) atMem->arr[1][2] = one;
        if (atMem->arr[2][2] == 0) atMem->arr[2][2] = one;
    }
    else if (atMem->arr[0][0] + atMem->arr[1][1] + atMem->arr[2][2] == two) // down slope diagonal danger
    {
        if (atMem->arr[0][0] == 0) atMem->arr[0][0] = one;
        if (atMem->arr[1][1] == 0) atMem->arr[1][1] = one;
        if (atMem->arr[2][2] == 0) atMem->arr[2][2] = one;
    }
    else if (atMem->arr[0][2] + atMem->arr[1][1] + atMem->arr[2][0] == two) // up slope diagonal danger
    {
        if (atMem->arr[0][2] == 0) atMem->arr[0][2] = one;
        if (atMem->arr[1][1] == 0) atMem->arr[1][1] = one;
        if (atMem->arr[2][0] == 0) atMem->arr[2][0] = one;
    }
    else return 0; // No block needed

    return 1; // Move was used to block
}

/*

*/
void playerOne()
{
    int turns; // Used later in while loop to temporarily hold the total number of turns
    int i,j;

    // Attempt to make FIFO
    if (mkfifo("xoSync", S_IRUSR | S_IWUSR) == -1)
    {
        // If FIFO already exists, continue...
        if (errno != EEXIST)
        {
            perror("mkfifo");
            exit(EXIT_FAILURE);
        } 
    }

    // Generate two random numbers for keys
    int rand1 = (rand() % 100) + 1;
    int rand2 = (rand() % 100) + 1;
    while (rand2 == rand1) rand2 = (rand() % 100) + 1; // Ensure rand1 and rand2 are different

    // Generate the System V keys using ftok
    key_t sharedMemKey = ftok("./xoSync",rand1);
    key_t semaphoreKey = ftok("./xoSync",rand2);

    // Create block of shared memory
    int sharedMemID = checkCall(shmget(sharedMemKey, sizeof(struct mSeg), IPC_CREAT | S_IRUSR | S_IWUSR), "shmget");

    // Create a semaphore set with size 2
    int semaphoreID = checkCall(semget(semaphoreKey, 2, IPC_CREAT | S_IRUSR | S_IWUSR), "shmget");

    // Initialize the semaphores in the set
    // Set player 1 semaphore to available and player 2 to in use
    initSemAvailable(semaphoreID, P1);
    initSemInUse(semaphoreID, P2);

    // Attach the segment of shared memory
    struct mSeg *atMem;
    atMem = (struct mSeg *) shmat(sharedMemID, NULL, 0);

    // initialize it
    atMem->turnCount = 0;
    for(i = 0;i < 3;i++)
        for(j = 0;j < 3;j++) atMem->arr[i][j] = 0;
            
    // Open the FIFO for "write"
    int fd = checkCall(open("xoSync", O_WRONLY), "open");

    // Write the previously generated random numbers to the FIFO in order
    checkCall(write(fd, &rand1, sizeof(int)), "writing to FIFO");
    checkCall(write(fd, &rand2, sizeof(int)), "writing to FIFO");

    // Close FIFO
    checkCall(close(fd), "close");

    // ****************** Enter game-play loop ***********************

    // While the turn counter is geater than -1...
    while (atMem->turnCount > -1)
    {
        // 1. reserve player 1's semaphore
        reserveSem(semaphoreID, P1);

        // 2. display the state of the game board
        displayBoard(atMem);

        // 3. make player 1's move
        switch(atMem->turnCount)
        {
            case 0: // One the first round, put an X in the center
            {
                atMem->arr[1][1] = 1;
                break;
            }
            case 1: // Second round...
            {
                // If player 2 is playing sub-optimally...
                if (atMem->arr[0][1] == -1 || atMem->arr[1][0] == -1 || atMem->arr[1][2] == -1 || atMem->arr[2][1] == -1)
                {
                    subOp = 1; // Set the subOp global variable to true

                    // If P2 put an O in the upper or lower edge, put an X in the left edge
                    if (atMem->arr[0][1] == -1 || atMem->arr[2][1] == -1)
                        atMem->arr[1][0] = 1;

                    // If P2 put an O in the left or right edge, put an X in the upper edge
                    else atMem->arr[0][1] = 1;
                }
                else // Assume P2 put an O in one of the corner squares
                {
                    // If O didn't play the top-left corner, put an X there
                    if (atMem->arr[0][0] != -1 && atMem->arr[2][2] != -1) atMem->arr[0][0] = 1; 

                    // Otherwise, put an X in the top-right corner
                    else atMem->arr[0][2] = 1; 
                    break;
                }
            }
            default:
            {
                // Look for win...
                if (winSearch(P1, atMem) == 1)
                {
                    printf("Player 1 \n");
                    atMem->turnCount = -1;
                }
                // Block if needed
                else if (blockSearch(P1, atMem) == 0)
                {
                    // If no block required and O is playing sub-optimally
                    if (subOp == 1)
                    {   
                        // If we have an X in the left edge, put an X in the top-left corner
                        if (atMem->arr[1][0] == 1) atMem->arr[0][0] = 1;

                        // Otherwise, if we have an X in the upper edge, put an X in the top-right corner
                        else if (atMem->arr[0][1] == 1) atMem->arr[0][2] = 1;
                    }

                    // Find last row or column that could possibly be a win for X
                    else if (atMem->arr[1][0] == 0 && atMem->arr[1][2] == 0) // mid row empty execpt X in center
                       atMem->arr[1][0] = 1;
                    else if (atMem->arr[0][1] == 0 && atMem->arr[2][1] == 0) // mid column empty execpt X in center
                        atMem->arr[0][1] = 1;

                    else randomPlace(P1,atMem); // Find any open square to place X (this is rarely used)
                }

                // If player 1 did not win this round, check if there are any moves left
                // If not, set the number of turns to -1
                // This check doesn't exist in player 2 because player 1 is always the last to play
                if (atMem->turnCount != -1)
                {
                    turns = atMem->turnCount;
                    atMem->turnCount = -1;
                    for(i = 0;i < 3;i++)
                    {
                        for(j=0;j<3;j++)
                        {
                            if (atMem->arr[i][j] == 0)
                            {
                                atMem->turnCount = turns;
                                break;
                            }
                        }
                        if (atMem->turnCount == turns) break;
                    }

                    if (atMem->turnCount == -1) printf("It's a draw!\n");
                }
                break;
            }
        }

        // 4. display the state of the game board
        displayBoard(atMem);

        // 6. release player 2's semaphore
        releaseSem(semaphoreID, P2);

        sleep(1);
    }
    // ****************** Exit game-play loop  ***********************

    // Open FIFO for write
    fd = checkCall(open("xoSync", O_WRONLY), "open");

    // Close the FIFO
    checkCall(close(fd), "close");

    // Detach the segment of shared memory
    shmdt(atMem);

    // Delete the shared memory and semaphores
    checkCall(shmctl(sharedMemID, IPC_RMID, NULL), "shmctl");
    checkCall(semctl(semaphoreID, 0, IPC_RMID), "semctl");

    exit(EXIT_SUCCESS);
}

/*

*/
void playerTwo()
{
    // Attempt to make FIFO
    if (mkfifo("xoSync", S_IRUSR | S_IWUSR) == -1)
    {
        // If FIFO already exists, continue...
        if (errno != EEXIST)
        {
            perror("mkfifo");
            exit(EXIT_FAILURE);
        } 
    }

    // Open FIFO for read
    int fd = checkCall(open("xoSync", O_RDONLY), "open");

    // Read two ints from FIFO
    int rand1, rand2;
    checkCall(read(fd, &rand1, sizeof(int)), "read");
    checkCall(read(fd, &rand2, sizeof(int)), "read");

    // Generate the System V keys using ftok
    key_t sharedMemKey = ftok("./xoSync",rand1);
    key_t semaphoreKey = ftok("./xoSync",rand2);

    // Retrieve the shared memory and the semaphore set
    int sharedMemID = checkCall(shmget(sharedMemKey, 0, 0), "shmget");
    int semaphoreID = checkCall(semget(semaphoreKey, 0, 0), "semget");

    // Attach the shared memory segment
    struct mSeg *atMem;
    atMem = (struct mSeg *) shmat(sharedMemID, NULL, 0);

    // ****************** Enter game-play loop ***********************
    while (1) // While true
    {
        // 1. reserve player 2's semaphore
        reserveSem(semaphoreID, P2);

        // 2. display the state of the game board
        displayBoard(atMem);

        // 3. if the turn counter is -1, exit loop
        if (atMem->turnCount == -1) break;

        // 4. make player 2's (O) move
        switch(atMem->turnCount)
        {
            case 0: // One the first round, put an O in one of the corners
            {
                int randSquare = (rand() % 4) + 1;
                if (randSquare == 1) atMem->arr[0][0] = -1;
                if (randSquare == 2) atMem->arr[0][2] = -1;
                if (randSquare == 3) atMem->arr[2][0] = -1;
                if (randSquare == 4) atMem->arr[2][2] = -1;
                
                break;
            }
            default: 
            {
                // Look for win...
                if (winSearch(P2, atMem) == 1)
                {
                    printf("Player 2 Wins!\n");
                    atMem->turnCount = -1;
                }
                // Block if needed
                else if (blockSearch(P2, atMem) == 0)
                {
                    // If no block required, place O
                    if (atMem->arr[0][0] == -1 && atMem->arr[0][2] == 0) atMem->arr[0][2] = -1;
                    else if (atMem->arr[0][2] == -1 && atMem->arr[2][2] == 0) atMem->arr[2][2] = -1;
                    else if (atMem->arr[2][0] == -1 && atMem->arr[0][0] == 0) atMem->arr[0][0] = -1;
                    else if (atMem->arr[2][2] == -1 && atMem->arr[0][2] == 0) atMem->arr[0][2] = -1;

                    else randomPlace(P2,atMem);
                }
            }
        }

        // 5. display the state of the game board
        displayBoard(atMem);

        // 6. increment the game turn by 1
        if (atMem->turnCount != -1) (atMem->turnCount)++;

        // 7. release player 1's semaphore
        releaseSem(semaphoreID, P1);

        sleep(1);
    }
    // ****************** Exit game-play loop  ***********************

    // Open FIFO for read
    fd = checkCall(open("xoSync", O_RDONLY), "open");

    // Close the FIFO
    checkCall(close(fd), "close");

    // Detach the segment of shared memory
    shmdt(atMem);
    
    exit(EXIT_SUCCESS);
}

/*

*/
int main(int argc, char *argv[])
{
    srand(time(NULL));
    if (argc == 1) printf("Please use the -1 or -2 flag to indicate the player.\n");
    else
    {
        if (!strcmp(argv[1], "-1")) { playerOne(); return 0;}
    if (!strcmp(argv[1], "-2")) { playerTwo(); return 0;}
    }

    exit(EXIT_SUCCESS);
}