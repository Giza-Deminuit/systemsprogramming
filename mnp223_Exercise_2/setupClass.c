/*
	Student Name: Maggie Pettus
    Student NetID: mnp223
    Compiler Used: GCC
    Program Description:
	This program sets up a pre-determined directory structure
    based on the "Systems Programming" course.
    The root directory name may be specified by the user via the CLI,
    or will default to "sysProg". 
*/

#include "includes.h"
#include <dirent.h>

// Allows the program to safely copy the contents of "str" into char pointer "dirname".
// This is primarily to ensure the first changeDir call has access to the correct
// root directory name, which may either be "sysProg" or whatever the user chose
// to specify.
char *dupStr(const char *str)
{
    char *cdup;
    int len;

    if (str == NULL)
        return NULL;
    
    len = strlen(str);
    cdup = (char *) calloc(len+1, sizeof(char));

    strcpy(cdup,str);

    return cdup;
}

// Attemps to open a directory with the name stored in "str" and
// gives it the predetermined permissions stored in "perms"
void makeDir(const char *str, mode_t perms)
{
    if (mkdir(str, perms) == -1)
    {
        perror("mkdir");
        exit(EXIT_FAILURE);
    }
    // v Optional code - uncomment to see the directories as they created
    //printf("\nDirectory created: %s\n", str);
}

// Attempts to change directories to the directory with the name stored
// in "str"
void changeDir(const char *str)
{
    if (chdir(str) == -1)
    {
        perror("chdir");
        exit(EXIT_FAILURE);
    }
    // v Optional code - uncomment to see the program as it changes directories
    // printf("\nChanged to directory: %s\n", str);
}

// Calls the "changeDir" function on both arguments
void changeDirPath(const char *str1, const char *str2)
{
    changeDir(str1);
    changeDir(str2);
}

// ****************************************************************************************************
//                                       M A I N    F U N C T I O N
// ****************************************************************************************************
int main(int argc, char *argv[])
{

    // Declare variables
    char *dirname; // Will hold directory names
    char stat_dirname[1024]; // Will hold strings of a pre-determined length
    mode_t perms = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP; // Permissions for directory creation


    // *********************************** Make root directory ****************************************

    // If the user did not specify a root name
    if (argc == 1)
    {
        // Copy string literal "sysProg" into dirname
        dirname = dupStr("sysProg");

        // Make the root directory with name "sysProg"
        makeDir("sysProg", perms);
    }
    // If the user specified a root name
    else if (argc == 2)
    {
        // Copy the contents of argv[1] into dirname 
        dirname = dupStr(argv[1]);

        // Make the root directory with the name given by user
        makeDir(dirname, perms);
    }
    // Otherwise,
    else
    {
        // if the user entered the wrong number of arguments, display error and abort
        perror("wrong num args");
        exit(EXIT_FAILURE);
    }
    // ***********************************************************************************************


    // Enter the root directory
    changeDir(dirname);
    
    // Make the three directories for the tests, programs, and exercises
    makeDir("exercise", perms);
    makeDir("program", perms);
    makeDir("test", perms);

    // Enter the "test" directory
    changeDir("test");
    
    // Make the three directories inside "test"
    for (int i = 0; i < 3; i++)
    {
        sprintf(stat_dirname,"test0%d", i+1); // test1, test2, test3 as i is incremented
        makeDir(stat_dirname, perms);
    }

    // Return to root directory and enter the "program" directory
    changeDirPath("..", "program");

    // Make the five directories inside "program"
    for (int i = 0; i < 5; i++)
    {
        sprintf(stat_dirname,"prog0%d", i+1);
        makeDir(stat_dirname, perms);
    }

    // Return to root directory and enter the "program" directory
    changeDirPath("..", "exercise");

    // Make the ten directories inside "exercise"
    for (int i = 0; i < 9; i++)
    {
        sprintf(stat_dirname,"exer0%d", i+1);
        makeDir(stat_dirname, perms);
    }
    
    sprintf(stat_dirname,"exer10");
    makeDir(stat_dirname, perms);
    
    // Free allocated memory
    free(dirname);

    exit(EXIT_SUCCESS);
}
