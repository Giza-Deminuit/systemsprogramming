#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ********************************************************************************************************
//                                                   N O D E    S T R U C T
// ********************************************************************************************************
struct Node
{
    // public by default
  	char *word;
  	struct Node *next; //Pointer to the NEXT Node
  	struct Node *prev;
};

// Initializes a new node with the word provided by user
void prependNode(struct Node* new_node, char *_word, struct Node* next_node);


// ********************************************************************************************************
//                                           l I N K E D    L I S T    S T R U C T
// ********************************************************************************************************
struct LinkedList
{
	// public by default
	struct Node *head;
	struct Node *tail;
};

// Initializes the new linked list
void initializeList(struct LinkedList *const list);

// Calls prependNode with the user-specified word and
// prepends the node to the list
void prepend(struct LinkedList* const list, char *_word);

// Displays the contents of each node from head to tail (reverse order from
// how they were entered)
void display(struct LinkedList* const list);

// Removes the head node from the list, freeing
// the memory allocated for both the node and
// the char array
int pop(struct LinkedList* const list);

// Destroys the doubly linked list and frees all
// associated memory
void destroyList(struct LinkedList *const list);

#endif