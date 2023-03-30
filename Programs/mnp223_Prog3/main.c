/*
	Student Name: Maggie Pettus
    Student NetID: mnp223
    Compiler Used: GCC
    Program Description:
    This program demonstrates the use of a parent and child
    process running at the same time.
    The parent simply echoes the user's input to the terminal,
    while the child process reads data from a file and outputs
    warnings when the values are too high or too low.
*/

#include "headers.h"

// Union - used to convert two-byte data element into four-byte integer
union ISTR
{
    int val;
    char bytes[4];
};

int valsReadFromFile; // Optional
time_t prev_warning_time; // Define a time variable
static pid_t pPid, cPid; // Helps us keep track of who's who
static int fileDescriptor = -1; // Holds the file descriptor of flowData.dat

// Check system calls for error
int checkCall(int val, const char *msg)
{
  if (val == -1)
    {
      if (errno == EINTR) return val; // we were interrupted, so
                                      // no reason to die here
      perror(msg);
      exit(EXIT_FAILURE);
    }
  return val;
}

// Parent exit handler
void pExit(void)
{
    char *out = "Warning! Pressure control monitoring has been terminated, exiting the system\n";
    checkCall(write(STDOUT_FILENO, out, strlen(out)), "write");

    char exitmsg[] = "Parent process is exiting...\n";
    checkCall(write(STDOUT_FILENO, exitmsg, strlen(exitmsg)), "write");
}

// Child exit handler
void cExit(void)
{
    if (fileDescriptor != -1)
        checkCall(close(fileDescriptor), "close");
    
    char *out = "\nChild is exiting ...\n\n";
    checkCall(write(STDOUT_FILENO, out, strlen(out)), "write");
}

//************************************ S I G N A L   H A N D L E R *******************************************
static void signalHandler(int sig)
{
    char buf[1024];

    // **************************** Handle change of child status ********************************
    // Handle change of status in child process
    if (sig == SIGCHLD)
    {
        char other[1024] = { 0 };
        pid_t waitReturn = 0;
        int status;

        // Wait for child process
        while ((waitReturn = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED )) > 0)
        {
            if (WIFEXITED(status)) // If the child exited normally,
            {
                // Exit from parent
                exit(EXIT_SUCCESS);
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

    // **************************** Handle Ctrl+C in parent ********************************
    if (sig == SIGINT)
    {
        checkCall(kill(cPid, SIGSTOP), "kill"); // Stop the child process

        const char output[] = "\nExit: Are you sure (Y/n)? ";
        const char incorrectIn[] = "\nIncorrect input: please enter 'Y' or 'n' ";
        char input[10];

        // Ask user if they want to exit
        checkCall(write(STDOUT_FILENO, output, strlen(output)), "write in SIGINT");
        
        // Record and respond to user input
        while(1)
        {
            // Read input from user
            checkCall(read(STDIN_FILENO, input, 10), "read in SIGINT");

            // If the user entered Y, terminate child
            if (input[0] == 'Y')
            {
                // End child process
                checkCall(kill(cPid, SIGCONT), "kill"); // resume the child 
                checkCall(kill(cPid, SIGTERM), "kill"); // kill the child
                break;
            }
                
            // If user entered n, exit loop and return to program
            else if (input[0] == 'n')
            {
                checkCall(kill(cPid, SIGCONT), "kill"); // resume the child 
                break;
            }
                
            // Otherwise, the user must have entered incorrect input
            checkCall(write(STDOUT_FILENO, incorrectIn, strlen(incorrectIn)), "write");
        }
    }

    // Handle child sending SIGUSR1 to parent
    if (sig == SIGUSR1)
    {
        int time_difference; // Seconds since last warning

        // Get time since last warning and convert to an int
        if (prev_warning_time != 0)
            time_difference = checkCall(time(NULL), "time") - prev_warning_time;
        else
            time_difference = 0;

        // Store current time
        prev_warning_time = checkCall(time(NULL), "time");
        
        sprintf(buf, "\nWarning! Line pressure is dangerously low. It has been %d seconds since the previous warning.\n", time_difference);
        
        // Display message for high values
        checkCall(write(STDOUT_FILENO, buf, strlen(buf)), "sig SIGUSR1 write");
    }

    // Handle child sending SIGUSR2 to parent
    if (sig == SIGUSR2)
    {
        int time_difference;

        // Get time since last warning and convert to an int
        if (prev_warning_time != 0)
            time_difference = checkCall(time(NULL), "time") - prev_warning_time; 
        else
            time_difference = 0;

        // Store current time
        prev_warning_time = checkCall(time(NULL), "time");

        sprintf(buf, "\nWarning! Line pressure is dangerously high. It has been %d seconds since the previous warning.\n", time_difference);

        // Display message for high values
        checkCall(write(STDOUT_FILENO, buf, strlen(buf)), "sig SIGUSR2 write");
    }
    if (sig == SIGALRM)
    {
        int numBytesRead; // used to store the number of bytes read from "flowData.dat"
        
        union ISTR in;
        in.val = 0; // Set union to 0
        checkCall(numBytesRead = read(fileDescriptor, in.bytes, 2), "read"); // Read a value from the flowData.dat file

        // If nothing was read, terminate
        if (numBytesRead == 0)
        {
            // ! Note that file is closed in child exit handler
            exit(EXIT_SUCCESS);
        }

        // Otherwise, if the value read was between 450 and 550,
        // inform parent of low value
        else if (in.val >= 450 && in.val <= 550)
            checkCall(kill(pPid,SIGUSR1), "kill");
        
        // Otherwise, if the value read was between 1450 and 1550,
        // inform parent of high value
        else if (in.val >= 1450 && in.val <= 1550)
            checkCall(kill(pPid,SIGUSR2), "kill");


        // Optional code. Uncomment to see the actual values read from the file
        /*sprintf(buf,"Value read: %d\n", in.val);
        checkCall(write(STDOUT_FILENO, buf, strlen(buf)), "write");*/

        // Optional code. Uncomment to see the number of values read from file
        /*valsReadFromFile++;
          sprintf(buf,"Number of values read from file so far: %d\n", valsReadFromFile);
        checkCall(write(STDOUT_FILENO, buf, strlen(buf)), "write"); */
    }
    if (sig == SIGTERM)
    {
        if (cPid == 0) exit(EXIT_SUCCESS);
    }
}

// ************************************************************
//                   M A I N    F U N C T I O N
// ************************************************************
int main(int argc, char *argv[])
{
    pPid = getpid(); // Get the parent process id

    // *************************** Set up signal handlers *************************
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_handler = signalHandler;
    sa.sa_flags = 0;

    // Add signals to mask
    checkCall(sigaddset(&sa.sa_mask, SIGINT), "sigaddsetSIGINT");
    checkCall(sigaddset(&sa.sa_mask, SIGCHLD), "sigaddsetSIGCHLD");
    checkCall(sigaddset(&sa.sa_mask, SIGALRM), "sigaddsetSIGALRM");

    // Change disposition for both parent and child before forking child
    checkCall(sigaction(SIGINT, &sa, NULL), "sigaction for SIGINT");
    checkCall(sigaction(SIGCHLD, &sa, NULL), "sigaction for SIGCHLD");
    checkCall(sigaction(SIGUSR1, &sa, NULL), "sigaction for SIGUSR1");
    checkCall(sigaction(SIGUSR2, &sa, NULL), "sigaction for SIGUSR2");
    checkCall(sigaction(SIGALRM, &sa, NULL), "sigaction for SIGALRM");
    checkCall(sigaction(SIGTERM, &sa, NULL), "sigaction for SIGTERM");

    // *************************** Set up timer *************************
    struct itimerval itmr;

    itmr.it_interval.tv_sec = 5;
    itmr.it_interval.tv_usec = 0;
    itmr.it_value.tv_sec = 5;
    itmr.it_value.tv_usec = 0;

    // *********************************** F O R K   P O I N T *************************************
    switch (cPid = fork()) // Fork child and get its process ID in parent, 0 in child
    {
        case -1: // Error
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        case 0: // Child
        {
            // add the exit handler
            if (atexit(cExit) != 0)
            {
                perror("atexit failure");
                exit(EXIT_FAILURE);
            }

            // Open file with read-only permission
            checkCall((fileDescriptor = open("flowData.dat", O_RDONLY)), "open");

            // ********************** Mask SIGINT ***********************************
            sigset_t sigset;
            sigemptyset(&sigset);
            checkCall(sigaddset(&sigset, SIGINT), "sigaddset");
            checkCall(sigprocmask(SIG_SETMASK, &sigset, NULL), "sigprocmask");

            // ******************* Set REAL Interval Timer ***************************
            checkCall(setitimer(ITIMER_REAL, &itmr, NULL), "setitimer in child");

            // Enter pause loop in child
            while (1) pause();
            break;
        }
        default: // Parent
        {
            // add the exit handler
            if (atexit(pExit) != 0)
            {
                perror("atexit failure");
                exit(EXIT_FAILURE);
            }

            // Reads input from user and echos it back to the terminal
            while (1)
            {
                char buf[1024];
                int numRead, numWrite;
                numRead = checkCall(read(STDIN_FILENO, buf, 1024), "read");
                if (numRead == -1) 
                    continue;

                while (1)
                {
                    numWrite = checkCall(write(STDOUT_FILENO, buf, numRead), "write");
                    if (numWrite == -1) 
                        continue; 
                    else 
                        break;
                }
            }
        }    
    }
}