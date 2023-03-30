#include "headers.h"

/* implement three threads ...
* print evens
* prints odds
* main thread */

int threadError(int err, char *str)
{
    if (err > 0)
    {
        errno = err;
        perror(str);
        exit(EXIT_FAILURE);
    }
    return err;
}

void *evenThread(void *args)
{
    int n = 0;
    struct timespec tv;
    pthread_t tid;

    tid = pthread_self();
    threadError(pthread_detach(tid), "pthread_detach");

    tv.tv_sec = 1;
    tv.tv_nsec = 0;
    for (n = 0; n < 61; n += 2)
    {
        printf("E: %d\n", n);
        nanosleep(&tv,NULL);
    }
    pthread_exit(NULL);
}

void *oddThread(void *args)
{
    int n = 1;
    struct timespec tv;
    pthread_t tid;

    tid = pthread_self();
    threadError(pthread_detach(tid), "pthread_detach");

    tv.tv_sec = 1;
    tv.tv_nsec = 0;
    for (n = 1; n < 61; n += 2)
    {
        printf("O: %d\n", n);
        nanosleep(&tv,NULL);
    }
    pthread_exit(NULL);
}

// Create threads join threads
int main(int argc, char *argv[])
{
    pthread_t tid[2]; // array of two thread ids
    int err;
    struct timespec tv;

    tv.tv_sec = 35;
    tv.tv_nsec = 0;

    // create threads
    threadError(pthread_create(&tid[0], NULL, evenThread, NULL), "pthread create");
    threadError(pthread_create(&tid[1], NULL, oddThread, NULL), "pthread create");

    // detach threads
    //threadError(pthread_detach(tid[0]), "pthread_detach");
    //threadError(pthread_detach(tid[1]), "pthread_detach");

    // join the threads
    //threadError(pthread_join(tid[0], NULL), "pthread join");
    //threadError(pthread_join(tid[1], NULL), "pthread join");
    
    nanosleep(&tv,NULL);
}