/*
	Student Name: Maggie Pettus
    Student NetID: mnp223
    Compiler Used: GCC
    Program Description:
	This program forks a single child process.
    The child process then generates a number
    between 1 and 100 (inclusive).
    Values greater than 75 and less than 25 are recorded
    and displayed. If a value between 48 and 51 is
    generated, the child exits.
*/

#include "headers.h"

// Helps us keep track of who's who
static pid_t pPid;
static pid_t cPid;

// Keep track of how many low and high values
static int numLowVals;
static int numHighVals;

// Check system calls for error
void checkCall(int val, const char *msg)
{
    if (val == -1)
    {
        perror(msg);
        exit(EXIT_FAILURE);
    }
}

// Turns timer on in child
void setTimeOn()
{
    // Set up interval timer
    struct itimerval timer;

    // Initialize structure for 15 second interval timer
    timer.it_value.tv_sec = 15;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 15;
    timer.it_interval.tv_usec = 0;

    if (setitimer(ITIMER_REAL, &timer, NULL) == -1)
    {
        perror("settimer");
        exit(EXIT_FAILURE);
    }
}

// Turns timer off in child
void setTimeOff()
{
    // Set up interval timerOff
    struct itimerval timerOff;

    // Initialize structure to zeros
    timerOff.it_value.tv_sec = 0;
    timerOff.it_value.tv_usec = 0;
    timerOff.it_interval.tv_sec = 0;
    timerOff.it_interval.tv_usec = 0;

    if (setitimer(ITIMER_REAL, &timerOff, NULL) == -1)
    {
        perror("settimer");
        exit(EXIT_FAILURE);
    }
}

// Generate a random number between 0 and 100
// and return it to SIGALRM signal handler
int generateNum()
{
    int randNum;
    randNum = rand() % (100);
    return randNum;
}


//*************************************** Child Only Handler *****************************************
static void handleCHLD(int sig)
{
    if (sig == SIGUSR1)
    {
        setTimeOff();
    }

    if (sig == SIGUSR2)
    {
        char msg[] = "Resumed...\n";
        checkCall(write(STDOUT_FILENO, msg, strlen(msg)), "write");
        setTimeOn();
    }

    if (sig == SIGTERM)
    {
        // If SIGTERM is sent from parent process, exit
        exit(EXIT_SUCCESS);
    }

    if (sig == SIGALRM)
    {
        int numGenReturn;
        char msgSigalrm[30] = { 0 };

        // Call function to generate random number
        numGenReturn = generateNum();

        // Display the number generated
        sprintf(msgSigalrm,"Child generated: %d\n",numGenReturn);
        write(STDOUT_FILENO, msgSigalrm, strlen(msgSigalrm));

        // Have the parent print the number of low values generated
        if (numGenReturn < 25)
        {
            kill(pPid, SIGUSR1);
        }

        // Have the parent print the number of high values generated
        else if (numGenReturn > 75)
        {
            kill(pPid, SIGUSR2);
        }

        // End the child process, sending SIGCHLD to parent
        else if (numGenReturn >= 48 && numGenReturn <= 51)
        {
            exit(EXIT_SUCCESS);
        }
    }
}

//*************************************** Parent Only Handler *******************************************
static void signalHandler(int sig)
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
                sprintf(other,"Child @ pid %d is exiting\n",waitReturn);
                write(STDOUT_FILENO, other, strlen(other));

                // Exit from parent
                char exitmsg[] = "Parent process is exiting...\n";
                checkCall(write(STDOUT_FILENO, exitmsg, strlen(exitmsg)), "write");
                exit(EXIT_SUCCESS);
            }
            if (WIFSIGNALED(status))
            {
                sprintf(other,"Child @ pid %d is exiting from a signal\n",waitReturn);
                write(STDOUT_FILENO, other, strlen(other));
            }
            if (WIFSTOPPED(status))
            {
                sprintf(other, "Child @ pid %d has paused\n",waitReturn);
                write(STDOUT_FILENO, other, strlen(other));
            }
            if (WIFCONTINUED(status))
            {
                sprintf(other, "Child @ pid %d has resumed\n",waitReturn);
                write(STDOUT_FILENO, other, strlen(other));
            }
        }

        // If waitpid returned an error...
        if (waitReturn == -1)
        {
            // If there are no child processes left, exit
            if (errno == ECHILD)
            {
                exit(EXIT_SUCCESS);
            }
            perror("waitpid");
            exit(EXIT_FAILURE);
        }
    }

    // Handle child sending SIGUSR1 to parent
    if (sig == SIGUSR1)
    {
        char buf[100];
        numLowVals++;
    
        // Convert number of values below 25 to a string
        sprintf(buf, "The child has generated %d value(s) less than 25\n\n", numLowVals);

        // Display message for low values
        checkCall(write(STDOUT_FILENO, buf, strlen(buf)), "sig SIGUSR1 write");
    }

    // Handle child sending SIGUSR2 to parent
    if (sig == SIGUSR2)
    {
        char buf[100];
        numHighVals++;

        // Convert number of values above 75 to a string
        sprintf(buf, "The child has generated %d value(s) greater than 75\n\n", numHighVals);

        // Display message for high values
        checkCall(write(STDOUT_FILENO, buf, strlen(buf)), "sig SIGUSR2 write");
    }

    // Handle Ctrl+C in parent
    if (sig == SIGINT)
    {
        // Have the child process stop the timer
        checkCall(kill(cPid, SIGUSR1), "kill");

        const char output[] = "\nExit: Are you sure (Y/n)? ";
        const char incorrectIn[] = "\nIncorrect input: please enter 'Y' or 'n' ";
        char input[10];

        // Ask user if they want to exit
        if ((write(STDOUT_FILENO, output, sizeof(output) -1)) == -1)
        {
        perror("write");
        exit(EXIT_FAILURE);
        }

        // Record and respond to user input
        while(1)
        {
            // Read input from user
            if ((read(STDIN_FILENO, input, 10)) == -1)
            {
                perror("read");
                exit(EXIT_FAILURE);
            }

            // If the user entered Y, terminate program
            if (input[0] == 'Y')
            {
                // End child process
                checkCall(kill(cPid, SIGTERM), "kill");
                sleep(1);
                exit(EXIT_SUCCESS);
            }
                
            // If user entered n, exit loop and return to program
            else if (input[0] == 'n')
            {
                // Restart timer
                kill(cPid, SIGUSR2);
                break;
            }
                
            // Otherwise, the user must have entered incorrect input
            if ((write(STDOUT_FILENO, incorrectIn, sizeof(incorrectIn) -1)) == -1)
            {
                perror("write");
                exit(EXIT_FAILURE);
            }
            
        }
    }
}
// **************************************** END SIGNAL HANDLERS ******************************


int main(int argc, char *argv[])
{
    // seeding for random number generation
    srand(time(0));

    pPid = getpid(); // Get the parent process id

    // *************************** Set up signal handlers *************************

    // ******** Parent ************
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_handler = signalHandler;
    sa.sa_flags = 0;

    checkCall(sigaddset(&sa.sa_mask, SIGINT), "sigaddsetSIGINT");

    // ******** Child ************
    struct sigaction sa_chld;

    sigemptyset(&sa_chld.sa_mask);
    sa_chld.sa_handler = handleCHLD;
    sa_chld.sa_flags = 0;

    checkCall(sigaddset(&sa_chld.sa_mask, SIGALRM), "sigaddsetSIGALRM");
    


    // *********************************** F O R K   P O I N T *************************************
    switch (cPid = fork())
    {
        case -1: // Error
            perror("fork");
            exit(EXIT_FAILURE);

        case 0: // Child
        {
            // Ignore SIGINT in child
            // The parent will handle whether the child needs to 
            // terminate or not after Ctrl+C
            sigset_t sigset;
            sigemptyset(&sigset);

            if (sigaddset(&sigset, SIGINT) == -1)
            {
                perror("sigaddset");
                exit(EXIT_FAILURE);
            }
            checkCall(sigprocmask(SIG_SETMASK, &sigset, NULL), "sigprocmask");

            
            // Set up handlers for child
            checkCall(sigaction(SIGTERM, &sa_chld, NULL), "sigaction for SIGTERM");
            checkCall(sigaction(SIGALRM, &sa_chld, NULL), "sigaction for SIGALRM");
            checkCall(sigaction(SIGUSR1, &sa_chld, NULL), "sigaction for SIGUSR1");
            checkCall(sigaction(SIGUSR2, &sa_chld, NULL), "sigaction for SIGUSR2");

            while (1)
            {
                // Set the timer
                setTimeOn();
                // Pause
                while (1);
                    pause();
            }
        }
        default: // Parent
        {
            // Set up handlers for parent
            checkCall(sigaction(SIGINT, &sa, NULL), "sigaction for SIGINT");
            checkCall(sigaction(SIGCHLD, &sa, NULL), "sigaction for SIGCHLD");
            checkCall(sigaction(SIGUSR1, &sa, NULL), "sigaction for SIGUSR1");
            checkCall(sigaction(SIGUSR2, &sa, NULL), "sigaction for SIGUSR2");
            // Pause
            while (1)
                pause();
        }    
    }
}