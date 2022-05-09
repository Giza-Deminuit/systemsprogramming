#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

// Initializes a new node with the word provided by user
void prependNode(struct Node* new_node, char *_word, struct Node* next_node)
{
    // Set the new node's word attribute to the word entered by the user
    new_node->word = malloc(strlen(_word) + 1);
    strcpy(new_node->word,_word);

    // Set the new node's next to the previous head node (NULL if this is the
    // first node in the list), then set its prev pointer to NULL
    new_node->next = next_node;
    new_node->prev = NULL;

    // then, if not NULL, set the previous head node's
    // prev to the new node
    if (next_node != NULL)
        next_node->prev = new_node;
}

// Initializes the new linked list by setting head and tail to NULL
void initializeList(struct LinkedList *const list)
{
	list->head = NULL;
	list->tail = NULL;
}

// Calls prependNode with the user-specified word and
// prepends the node to the list
void prepend(struct LinkedList* const list, char *_word)
{
    // Create a new node and include it in the list
    struct Node *new_node = (struct Node*)malloc(sizeof(struct Node));
    prependNode(new_node, _word, list->head);

    // If the list was empty before, assign the new node as 
    // both the head and the tail
    if (list->head == NULL)
        list->tail = new_node;
    list->head = new_node;

}

// Displays the contents of each node from head to tail
void display(struct LinkedList* const list)
{
    // If the list is empty, inform user and return
    if (list->head == NULL)
    {
        printf("The list is empty.\n");
        return;
    }
    // Create temp node pointer and set it to the head node
    struct Node *temp = list->head;

    // While the end of the list hasn't been reached,
    // print the word in the node pointed to by temp,
    // and set temp to the next node
    while(temp != NULL)
    {
        printf("%s\n", temp->word);
        temp = temp->next;
    }
    
}

// Removes the head node of the list
int pop(struct LinkedList* const list)
{
    // Create temp node pointer and set it to the head node
    struct Node *temp = list->head;

    // If the list is empty, return -1 to indicate failure
    if (temp == NULL)
        return -1;
    
    // Set the head pointer to the next node in the list,
    // free the memory allocated by the char array "word",
    // free the memory allocated for the head node
    list->head = list->head->next;
    free(temp->word);
    free(temp);

    // Return 0 to indicate success
    return 0;
}

// Destroys the linked list and frees all associated memory
void destroyList(struct LinkedList* const list)
{
    // Call the pop function until it returns failure (list is empty)
    while (pop(list) != -1) {};

    // Free the memory allocated for the linked list structure
    free(list);
}