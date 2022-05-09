/* All of the includes for the project go here … 
 * those includes must support signals, interval timers,
 * read, open, and close. You will also need support for 
 * error numbers
 */

/* Problem 1  */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>


#define BUF_SIZE 1024

static int timed;

/* the myPrint function takes a constant c-string
 * and outputs it to STDOUT using write
 * myPrint returns 0 on failure and 1 on success
 */
int myPrint(const char *str)
{
  /* Problem 2 */
  if ((write(STDOUT_FILENO, str, strlen(str))) == strlen(str))
    return 1;
  else
    return 0;
}

/* the myPrintInt function takes a constant int
 * and outputs it to STDOUT using write
 * myPrintInt returns 0 on failure and 1 on success
 */
int myPrintInt(const int val)
{
/* Problem 3 */
  char string[5];

  sprintf(string, "%d", val);

  if (write(STDOUT_FILENO, string, 5) > 0)
    return 1;
  else
    return 0;
}

/* implement the signal handler below ...
 * use the provided prototype
 * It must handle two signals, SIGINT and SIGALRM
 */
void signalHandler(int sig)
{
/* Problem 4 */
  if (sig == SIGINT)
  {
    const char output[] = "\nExit: Are you sure (Y/n)? ";
    const char incorrectIn[] = "\nIncorrect input: please enter 'Y' or 'n' ";
    char input[50];

    if ((write(STDOUT_FILENO, output, sizeof(output) -1)) == -1)
    {
      perror("write");
      exit(EXIT_FAILURE);
    }

    while(1)
    {
      // Read input from user
      if ((read(STDIN_FILENO, input, 50)) == -1)
      {
        perror("read");
        exit(EXIT_FAILURE);
      }

      // If the user entered Y, terminate program
      if (input[0] == 'Y')
        exit(EXIT_SUCCESS);
      // If user entered n, exit loop and return to program
      else if (input[0] == 'n')
        break;
      // Otherwise, the user must have entered incorrect input
      if ((write(STDOUT_FILENO, incorrectIn, sizeof(incorrectIn) -1)) == -1)
      {
        perror("write");
        exit(EXIT_FAILURE);
      }
    }
  }

  // set timedout flag to 1
  if (sig == SIGALRM)
    timed = 1;
}
/* This function reads a line of text from a file
 * we are simulating another functions behavior here ..
 * Return a 0 if no characters were read, return a 1 otherwise
 */
int readLine(int fd, char *line)
{
/* Problem 5 */

  int i = 0; // Used to keep track of how many chars are in "line"
  ssize_t numRead;

  while(1)
  {
    if ((numRead = read(fd, (line + i), sizeof(char))) == -1)
    {
      perror("read");
      exit(EXIT_FAILURE);
    }

    // If no characters were read, return 0
    if (numRead == 0)
      return 0;
    
    // If the last character read was a newline or a carriage return,
    // replace it in the array with a null char and return 1
    if (line[i] == '\n' || line[i] == '\r')
    {
        line[i] = '\0';
        return 1;
    }

    i++; // Increment the number of chars in the array
  }
}

/* This function reads a question answer pairing
 * from the provided pair of file descriptors
 * It returns 0 if the files are empty
 * and 1 if it successfully reads the pairing
 */

int readQA(int questFd, int ansFd, char *quest, char *ans)
{
  if (readLine(questFd, quest) == 0) return 0; 
  if (readLine(ansFd, ans) == 0) return 0;
  return 1;
}

int main(int argc, char *argv[])
{

  int numRead = 0; // Holds number of bytes read
  int numWrite = 0; // Holds number of bytes written
  int question = 1; // Keeps track of which "number" question we're on
  int correct = 0; // Keeps track of how many questions user has gotten right
  char buf[BUF_SIZE]; // Standard I/O buffer
  char quest[BUF_SIZE]; // Holds the question text from file
  char ans[BUF_SIZE]; // Holds the answer text from file
  int questFd, ansFd; // Holds the file descriptors for the respective answer/question files

 /* Problem 6a*/
  struct sigaction hndlr;
  struct itimerval tvOn;
  struct itimerval tvOff;

  /* Problem 6b */

  // Set sa_hanlder to our custom signalHandler function
  hndlr.sa_handler = signalHandler;

  // Start the mask as empty
  sigemptyset(&hndlr.sa_mask);
  hndlr.sa_flags = 0;


  // Add SIGINT and SIGALRM to the mask
  if (sigaddset(&hndlr.sa_mask, SIGINT) == -1)
  {
    perror("sigaddset");
    exit(EXIT_FAILURE);
  }

  if (sigaddset(&hndlr.sa_mask, SIGALRM) == -1)
  {
    perror("sigaddset");
    exit(EXIT_FAILURE);
  }


  /* Problem 6c */

  // Initialize structure for 30 second timer
  tvOn.it_value.tv_sec = 30;
  tvOn.it_value.tv_usec = 0;
  tvOn.it_interval.tv_sec = 0;
  tvOn.it_interval.tv_usec = 0;

  /* Problem 6d */
  tvOff.it_value.tv_sec = 0;
  tvOff.it_value.tv_usec = 0;
  tvOff.it_interval.tv_sec = 0;
  tvOff.it_interval.tv_usec = 0;

  /* Problem 7 */
  if (sigaction(SIGINT, &hndlr, NULL) == -1)
  {
    perror("sigaction");
    exit(EXIT_FAILURE);
  }

  if (sigaction(SIGALRM, &hndlr, NULL) == -1)
  {
    perror("sigaction");
    exit(EXIT_FAILURE);
  }
  

/* Problem 8  */

  // Open quest.txt
  if ((questFd = open("quest.txt", O_RDONLY)) == -1)
  {
    perror("open");
    exit(EXIT_FAILURE);
  }

  // Open ans.txt
  if ((ansFd = open("ans.txt", O_RDONLY)) == -1)
  {
    perror("open");
    exit(EXIT_FAILURE);
  }
    
   /* this loop handles the Q/A stuff 
    * I have included some of it to make you life simpler 
    * I have also left some commnents to assist you as well 
    *
    * read the first question, answer pairing prior to entering the loop
    */
    readQA(questFd, ansFd, quest, ans);
  while (1)
  {
      /* output the current question */
      if (myPrint("#") == 0)
      {
        perror("write");
        exit(EXIT_FAILURE);
      }
    
      if (myPrintInt(question) == 0)
      {
        perror("write");
        exit(EXIT_FAILURE);
      }

      if (myPrint(" ") == 0)
      {
        perror("write");
        exit(EXIT_FAILURE);
      }


      if (myPrint(quest) == 0)
      {
        perror("write");
        exit(EXIT_FAILURE);
      }


      if (myPrint("? ") == 0)
      {
        perror("write");
        exit(EXIT_FAILURE);
      }


      /* we will set the timedout flag to 0, since it hasn't yet */
      timed = 0;

      /* now set the interval timer prior to reading in the user's response */

      /*  Problem 9 set the interval timer to its active delay structure */
      if (setitimer(ITIMER_REAL, &tvOn, NULL) == -1)
      {
         perror("settimer");
         exit(EXIT_FAILURE);
      }


      /* read in the user's response, this is systems programming so don't use 
       * scanf */
      /* Problem 10 */

      // Not using an error check on this one because we need the -1 for
      // following code
      numRead = read(STDIN_FILENO, buf, BUF_SIZE);
      

      /* test to see if the user responded and if an error has occurred 
       * an error can actually occur as part of the functionality of this program 
       * both SIGINT and SIGALRM will cause the read call to return a -1 
       * this can be recognized by testing for a specific error number ...  
       * The error is known as interrupted systems call 
       * Should this be the case, do the following .... 
       * If the read call was interrupted and the timedout flag isn't set 
       * just continue 
       * if the timedout flag was set, inform the user that time is up 
       * then go to the next question
       */
      if (numRead == 0) break; // breaks out of while(1) loop
      if (numRead == -1)
	    {
	      if (errno == EINTR)
	      {
	        if (timed)
		      {
		        if (myPrint("\nTime's up, next question\n") == 0)
            {
              perror("write");
              exit(EXIT_FAILURE);
            }

		        if (readQA(questFd, ansFd, quest, ans) == 0) break; // If EOF, break out of loop
		        question++;
		      }
	        continue; // Restart loop
	      }
	      perror("read");
	      exit(EXIT_FAILURE);
	    }
      /* disable the timer here */
      /* Problem 11*/
      if (setitimer(ITIMER_REAL, &tvOff, &tvOn) == -1)
      {
         perror("settimer");
         exit(EXIT_FAILURE);
      }

      /* we will convert the buf being read in to a c-string by tacking on a 0 */
      buf[numRead-1] = 0;
      /* check the answer */
      if (strcmp(buf,ans) == 0)
      {
        correct++;

        if (myPrint("\ncorrect\n") == 0)
        {
          perror("write");
          exit(EXIT_FAILURE);
        }

      } else
      {
        if (myPrint(ans) == 0)
        {
          perror("write");
          exit(EXIT_FAILURE);
        }

        if (myPrint("\nwrong\n") == 0)
        {
          perror("write");
          exit(EXIT_FAILURE);
        }
      }
      /* read the next question .. break if there are no more questions */
      if (readQA(questFd, ansFd, quest, ans) == 0) break;

      question++;
  }

      if (myPrint("final score is ") == 0)
      {
        perror("write");
        exit(EXIT_FAILURE);
      }

      if (myPrintInt(correct) == 0)
      {
        perror("write");
        exit(EXIT_FAILURE);
      }
      if (myPrint(" out of ") == 0)
      {
        perror("write");
        exit(EXIT_FAILURE);
      }
      if (myPrintInt(question) == 0)
      {
        perror("write");
        exit(EXIT_FAILURE);
      }
      
      if ((write(STDOUT_FILENO, "\n", 1)) == -1)
      {
        perror("write");
        exit(EXIT_FAILURE);
      }

      /* Problem 12 */
      if (close(questFd) == -1)
      {
        perror("close");
	      exit(EXIT_FAILURE);
      }

      if (close(ansFd) == -1)
      {
        perror("close");
	      exit(EXIT_FAILURE);
      }
}
