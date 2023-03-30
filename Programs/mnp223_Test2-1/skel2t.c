/* Write a threaded program that does the following
   1. Prompts the user for the number of integers to enter
   2. Prompts the user for each integer and writes them into 
      a file named data.dat
   3. Determines how many integers are > 100
   4. Determines how many integers are < 100
   5. Outputs the total count for each group.

   The program should perform this task in the following way:
   Create a producer thread that:
   1. Prompts the user for the number of integers to enter
   2. Prompts the user for each integer and writes them into a file
      named data.dat
   Create a consumer thread that:
   1. Determines how many integers are > 100
   2. Outputs that value
   3. Sets that value to its exit value
   Create a consumer thread that:
   1. Determines how many integers are < 100
   2. Outputs that value
   3. Sets that value to its exit value
   Create a main thread that: 
   1. Creates threads 1 - 3
   2. Waits on the values of threads 2 and 3. 
   3. Outputs the values from threads 2 and 3.
 */

/* Include files */
#include <stdio.h>
#include <stdlib.h>
/* Problem 1. Add the necessary include files for this program */
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <malloc.h>


/* 
   Global variables:
   We will need a mutex, a condition variable, and a predicate variable. 
   Recall that the predicate variable is the variable we use to determine
   whether data was available prior to our first call to pthread_cond_wait

 */

/* Problem 2. Declare the global variables for the predicate variable, 
   the mutex, and the condition variable. Do not forget to initialize 
   the mutex and the condition variable 
 */
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cnd = PTHREAD_COND_INITIALIZER;
static int predicate;

/* This is a convenience function for dealing with errors
   and threads
 */

void hndlError(int error, const char *str)
{
  if (error == 0) return;
  errno = error;
  perror(str);
  exit(EXIT_FAILURE);
}

// Check system calls for error
void checkCall(int val, const char *msg)
{
    if (val == -1)
    {
        perror(msg);
        exit(EXIT_FAILURE);
    }
}

/* Define the three thread start functions. 
   You can name them whatever you wish 
 */

/* Problem 3. Define and write the start function for thread 1 */
void *thread1Func(void *args)
{
    // Detach self
    pthread_t tid = pthread_self();
    hndlError(pthread_detach(tid), "pthread_detach");

    int intstoenter, input, fd;
    char *filename = "data.dat";
    int openFlags = O_CREAT | O_WRONLY | O_TRUNC;
    mode_t filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

    printf("How many integers do you want to enter? ");
    scanf("%d", &intstoenter);

    // Lock mutex
    hndlError(pthread_mutex_lock(&mtx), "pthread_mutex_lock");

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

    predicate = 1;
    hndlError(pthread_mutex_unlock(&mtx),"pthread_mutex_unlock");
    hndlError(pthread_cond_broadcast(&cnd), "pthread_cond_broadcast");

    pthread_exit(NULL);
}



/* Problem 4. Define and write the start function for thread 2 */
void *thread2Func(void *args)
{
    // Declare and allocate space for counter
    int *large = malloc(sizeof(int));
    *large = 0;

    hndlError(pthread_mutex_lock(&mtx), "pthread_mutex_lock");

    // Loop while predicate var is zero
    while (predicate == 0)
    {
        hndlError(pthread_cond_wait(&cnd,&mtx), "cond_wait");
    }

    // open file data.dat for reading
    int fd = open("data.dat", O_RDONLY);
    if (fd == -1)
    {
        perror("Opening file for read");
        exit(EXIT_FAILURE);
    }

    hndlError(pthread_mutex_unlock(&mtx),"pthread_mutex_unlock");

    // Read each value from the file
    int val, inBytes;
    while (inBytes = read(fd, &val, sizeof(int)) > 0)
    {
        if (val > 100) (*large)++;
    }
    if (inBytes == -1)
    {
        perror("read");
        exit(EXIT_FAILURE);
    }

    checkCall(close(fd), "close"); // Close file
    printf("total larger: %.0d\n", *large); // Display total larger
    pthread_exit(large);
}


/* Problem 5. Define and write the start function for thread 3 */
void *thread3Func(void *args)
{
    // Declare and allocate space for counter
    int *small = malloc(sizeof(int));

    hndlError(pthread_mutex_lock(&mtx), "pthread_mutex_lock");

    // Loop while predicate var is zero
    while (predicate == 0)
    {
        hndlError(pthread_cond_wait(&cnd,&mtx), "cond_wait");
    }

    // open file data.dat for reading
    int fd = open("data.dat", O_RDONLY);
    if (fd == -1)
    {
        perror("Opening file for read");
        exit(EXIT_FAILURE);
    }

    hndlError(pthread_mutex_unlock(&mtx),"pthread_mutex_unlock");

    // Read each value from the file
    int val, inBytes;
    while (inBytes = read(fd, &val, sizeof(int)) > 0)
    {
        if (val < 100) (*small)++; 
    }
    if (inBytes == -1)
    {
        perror("read");
        exit(EXIT_FAILURE);
    }
    checkCall(close(fd), "close"); // Close file
    printf("total smaller: %.0d\n", *small); // Display total smaller
    pthread_exit(small);
}

/* Problem 6: Define and write the function for the main thread */
int main(int argc, char *argv[])
{
    // Create threads 2, 3,and 1
    pthread_t thread2, thread3, thread1;
    hndlError(pthread_create(&thread2, NULL, thread2Func, NULL),"pthread_create");
    hndlError(pthread_create(&thread3, NULL, thread3Func, NULL),"pthread_create");
    hndlError(pthread_create(&thread1, NULL, thread1Func, NULL),"pthread_create");

    // Join thread 2 and save its return value
    void *retlarge;
    int *large;
    hndlError(pthread_join(thread2, &retlarge),"pthread_join");
    large = (int *)retlarge;
    

    // Join thread 3 and save its return value
    void *retsmall;
    int *small;
    hndlError(pthread_join(thread3, &retsmall),"pthread_join");
    small = (int *)retsmall;

    printf("larger: %.0d\nsmaller: %.0d\n",*large,*small); // Output final results

    // Free allocated data
    free(retlarge);
    free(retsmall);
   
  exit(EXIT_SUCCESS);
}