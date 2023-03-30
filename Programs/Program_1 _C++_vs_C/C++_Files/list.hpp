#pragma once

#include <iostream>
#include <string>

using namespace std;

// ********************************************************************************************************
//                                                     N O D E    C L A S S
// ********************************************************************************************************
class Node
{
	public:
  		string word; // String to hold user input
  		Node *next; //Pointer to the NEXT Node
  		Node *prev; //Pointer to the PREV Node
  		
  		Node(); //default constructor
 		Node(string); // constructor with string
};



// ********************************************************************************************************
//                                           l I N K E D    L I S T    C L A S S
// ********************************************************************************************************
class LinkedList
{
	private:
		Node *head; // Pointer to head of the list
		Node *tail; // Pointer to tail of the list
	public:
		LinkedList(); // Default constructor
		~LinkedList(); // Destructor
		
		void prepend(string word); // Add new node with user input to front of list
		void display();	// Display contents of list
};