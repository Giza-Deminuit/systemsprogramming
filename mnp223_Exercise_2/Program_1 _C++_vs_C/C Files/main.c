/*
	Student Name: Maggie Pettus
    Student NetID: mnp223
    Compiler Used: GCC
    Program Description:
	This program reads single-word input from the keyboard and
	inserts each into a doubly-linked list until "STOP" is entered.
	The words entered (not including "STOP") are then displayed 
	back to the console in reverse order that they were entered. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

int main()
{
	// Declare char array to hold user input, create linked list data structure, 
	// and initialize its attributes
	// Fun fact: "The 45-letter word pneumonoultramicroscopicsilicovolcanoconiosis is the 
	// longest English word that appears in a major dictionary." - Wikipedia
    char str[45]; 
	struct LinkedList *const list = (struct LinkedList*)malloc(sizeof(struct LinkedList));
	initializeList(list);
	
	printf("This program will read strings until 'STOP' is entered.\n");

	// While input is read from the keyboard,
	while(scanf("%s", str))
	{
		// test whether the user entered "STOP"
		if (strcmp(str,"STOP") == 0) // strcmp returns 0 if the args are equal
		{
			// If "STOP" was entered,
			printf("\nThe data you entered will now be displayed in reverse\n");

			// Pause
			getchar();
			getchar();

			// Display the contents of the list, and free the memory
			display(list);
			destroyList(list);

			// Exit while loop
			break;
		}
		// If the user did not enter "STOP", prepend the input
		// into the list
		else
		{
			prepend(list, str);
		}	
	}
	
	// Exit program
	exit(EXIT_SUCCESS);
}