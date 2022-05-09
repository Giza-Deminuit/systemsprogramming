#include <iostream>
#include <string>
#include "list.hpp"

using namespace std;

// ***************************************************************************************************************
//                                         N O D E   C L A S S   M E T H O D S
// ***************************************************************************************************************

// Default constructor method
Node::Node()
{
  word = ""; // sets defalt data to null string
  next = nullptr; //sets next node to a NULL pointer
  prev = nullptr; // sets prev node to a NULL pointer
}

// Constructor with string for word
Node::Node(string _word)
{
  word = _word;
  next = nullptr; //sets next node to NULL pointer
  prev = nullptr; // sets prev node to a NULL pointer
}



// ***************************************************************************************************************
//                                L I N K E D L I S T    C L A S S    M E T H O D S
// ***************************************************************************************************************

// Method for the LinkedList default constructor
LinkedList::LinkedList()
{
  head = nullptr; //sets head node to a NULL pointer
  tail = nullptr; //sets tail node to a NULL pointer
}

// Method for the LinkedList destructor
LinkedList::~LinkedList() 
{
   Node *tmp = head; // Set tmp to the head of the list
   
   while (tmp != nullptr) // Loop while there are nodes in the list
   {
   		head = head->next; // Move the head to the next node
   		
   		 delete tmp; // delete the current node saved in tmp
   		
   		tmp = head; // Set tmp to the new head
   }
   
   tail = nullptr; // Set tail to nulltpr
}

// Method for the prepend function
void LinkedList::prepend(string word)
{
	Node *tmp = new Node(word); // Create the new Node
	
	tmp->next = head; // Set tmp's next to the current head
	
	if (head == nullptr) // Check if the list is empty
	{
		tail = tmp; // If it is, set tail to the new Node
	}
	else //otherwise
	{
		head->prev = tmp; // set head's previous to the new node
	}
	
	head = tmp; // Move the head to the new Node
}

void LinkedList::display()
{
	if (head == nullptr) // Check to see if list is empty
	{
		cout << "The list is empty." << endl; // If it is, inform user
		return; // and return
	}
	
	Node *tmp = head; // If the list isn't empty, create a pointer to Node and set it to the head Node
	
	while (tmp != nullptr) // Loop while tmp is still in the list
	{
		// Display the contents of the doubly linked list in reverse order that they were entered
		cout << tmp->word << endl;
		
		tmp = tmp->next; // Move tmp to the next node
	}
}