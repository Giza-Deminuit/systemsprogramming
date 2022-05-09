/*
	Student Name: Maggie Pettus
    Student NetID: mnp223
    Compiler Used: GCC
    Program Description:
	This program reads input from either a file
    or the terminal, counts the words and/or characters
    read, and displays them. 
*/

#include "includes.h"

#define BUF_SIZE 1024

// Checks for system call error
void checkCall(int val, const char *msg)
{
    if (val == -1)
    {
        perror(msg);
        exit(EXIT_FAILURE);
    }
}

// Manages counting words and characters from STDIN_FILENO
void wordCountIn(int fd, char *buf, int *chars, int *words)
{
    ssize_t numRead;
    int i = 0;
    
    // While the user hasn't entered CTRL+d...
    while ((numRead = read(fd, buf, BUF_SIZE-1)) != 0)
    {
        if (numRead == -1)
        {
            perror("read");
            exit(EXIT_FAILURE);
        }
        // Keep running total on number of chars
        *chars += numRead;
        
        // Add a nul terminator to end of input
        // to ensure the end of the string is 
        // clear
        buf[numRead] = '\0';

        i = 0; // Reset to 0 after each read
        
        // Keep looping through the input until the null
        // terminator is found
        while (buf[i] != '\0')
        {
            // When a word is found, iterate through
            // all its characters
            while (isgraph(buf[i]))
                i++;

            // When the word ends, increment the number
            // of words
            if (isgraph(buf[i-1]))
                (*words)++;

            // If the current character isn't the null terminator,
            // increment i to proceed with loop
            if (buf[i] != '\0')
                i++;   
        }
    }
}


// Manages counting words and characters from a file
void wordCountFile(int fd, char *buf, int *chars, int *words)
{
    ssize_t numRead;
    int i = 0;

    // Read all contents from file
    numRead = read(fd, buf, BUF_SIZE-1);
    if (numRead == -1)
        {
            perror("read");
            checkCall(close(fd), "close");
            exit(EXIT_FAILURE);
        }

    // Add null terminator to end of read input
    buf[numRead] = '\0';

    // Set number of characters read
    *chars = numRead;

    // While the end of the input hasn't been reached,
    while (buf[i] != '\0')
    {
        // If the current index is blank, increment
        // and return to start of loop
        if (isblank(buf[i]) && words == 0)
        {
            i++;
            continue;
        }

        // If the end of a word has been found, increment words
        if (buf[i] == ' ' || buf[i] == '\t' || buf[i]== '\r' || buf[i] == '\n')
        {
            if (isgraph(buf[i-1]))
                (*words)++;

            // Loop through to next word
            while (buf[i] == ' ' || buf[i] == '\t' || buf[i]== '\r' || buf[i] == '\n')
                i++;
        }
        i++;
    }
    
    
}

// This function reads the arguments entered by the user and sets the 
// appropriate flags. On success, it will return 1 if a filename was read, and a
// 0 otherwise.
int setFlags(int *wFlag, int *cFlag, int *fileFlag, char *arg, int argc)
{
    if (strcmp(arg,"-w") == 0)
    {
        *wFlag = 1;
    }
        
    else if (strcmp(arg,"-c") == 0)
    {
        *cFlag = 1;
    }
        
    else if (strcmp(arg,"-wc") == 0)
    {
        *wFlag = 1;
        *cFlag = 1;
    }
    else if (strcmp(arg,"-cw") == 0)
    {
        *wFlag = 1;
        *cFlag = 1;
    }

    // If the argument doesn't start with a '-' it is 
    // assumed to be the filename
    else if (arg[0] != '-')
    {
        
        // If a filename was already entered,
        // print error and exit
        if (*fileFlag == 1)
        {
            perror("\nUnexpected argument");
            exit(EXIT_FAILURE);
        }

        *fileFlag = 1;

        // If only the filename was entered, set c and w flags
        if (argc == 3)
        {
            *wFlag = 1; 
            *cFlag = 1;
        }
            
        return 1;
    }
    // If the argument doesn't match any of the supported
    // commands, print error and exit
    else
    {
        perror("\nUnexpected argument");
        exit(EXIT_FAILURE);
    }

    return 0;
}

// ******************************************************************************************
//                               M A I N    F U N C T I O N
// ******************************************************************************************
int main(int argc, char *argv[])
{
    // If the user entered too many or too few arguments, exit program
    if (argc > 5)
    {
        perror("\nERROR: too many args\n");
        exit(EXIT_FAILURE);
    }
    if (argc < 2)
    {
        perror("\nERROR: not enough args -did you use 'wc'?\n");
        exit(EXIT_FAILURE);
    }

    int wFlag = 0; // Set to 1 if -w is read
    int cFlag = 0; // Set to 1 if -c is read
    int fileFlag = 0; // Set to 1 if an argument not starting with '-' is read;
                      // this is assumed to be the name of a file

    char buf[BUF_SIZE]; // Standard I/O buffer
    char argFN[50]; // holds filename
    char argWC[30]; // holds 'wc'
    char arg1[30]; // holds first argument
    char arg2[30]; // holds second argument
    char arg3[30]; // holds third argument
    char filename[30]; // Holds filename
    int fd; // file descriptor
    int chars = 0; // Holds number of chars read
    int words = 0; // Holds number of words read

    // Copy the arguments into char arrays to make them
    // easier to work with
    strcpy(argWC, argv[1]);
    if (argc > 2)
        strcpy(arg1, argv[2]);
    if (argc > 3)
        strcpy(arg2, argv[3]);
    if (argc > 4)
        strcpy(arg3, argv[4]);
    
    // Make sure the user entered the 'wc' command first
    if (strcmp(argWC,"wc") != 0)
    {
        perror("ERROR: must enter 'wc' command");
        exit(EXIT_FAILURE);
    }
    
    // Based on the number of arguments entered from the command line,
    // call the setFlags function on each argument and open the text
    // file if one is specified.
    if (argc == 2)
    {
        cFlag = wFlag = 1;
    }
    if (argc > 2)
        if (setFlags(&wFlag, &cFlag, &fileFlag, arg1, argc) == 1)
            strcpy(filename, arg1);
    if (argc > 3)
        if (setFlags(&wFlag, &cFlag, &fileFlag, arg2, argc) == 1)
            strcpy(filename, arg2);
    if (argc > 4)
        if (setFlags(&wFlag, &cFlag, &fileFlag, arg3, argc) == 1)
            strcpy(filename, arg3);
        

    // If no filename was entered, call wordCountIn
    // with STDIN_FILENO as the file descriptor
    if (fileFlag == 0)
        wordCountIn(STDIN_FILENO, buf, &chars, &words);

    // Otherwise, open the file specified by the user
    // and call wordCountFile
    else
    {
        checkCall((fd = open(filename, O_RDONLY)), "open");
        wordCountFile(fd, buf, &chars, &words);
    }
        
    

    // Display both words and characters
    if (wFlag + cFlag == 2)
    {
        // Display filename if there is one
        if (fileFlag == 1)
            printf("\n%d  %d  %s\n", words, chars, filename);
        else
            printf("\n%d  %d\n", words, chars);
    }
    // Display just words
    else if (wFlag == 1)
    {
        // Display filename if there is one
        if (fileFlag == 1)
            printf("\n%d  %s\n", words, filename);
        else
            printf("\n%d\n", words);
    }
    // Display just characters
    else if (cFlag == 1)
    {
        // Display filename if there is one
        if (fileFlag == 1)
            printf("\n%d  %s\n", chars, filename);
        else
            printf("\n%d\n", chars);
    }

    // If a file was opened, close it
    if (fileFlag == 1)
        checkCall(close(fd), "close");

    exit(EXIT_SUCCESS);
}