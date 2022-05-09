/*
	Student Name: Maggie Pettus
    Student NetID: mnp223
    Compiler Used: G++
    Program Description:
	This program reads single-word input from the keyboard and
	inserts each into a doubly-linked list until "STOP" is entered.
	The words entered (not including "STOP") are then displayed 
	back to the console in reverse order that they were entered. 
*/

#include <iostream>
#include <string>
#include "list.hpp"

using namespace std;

int main()
{
	// Declare variables
	LinkedList list;
	string word;
	
	cout << "This program will read strings until 'STOP' is entered.\n";
	
	// While input is read from keyboard,
	while (cin >> word)
	{
		// if the input == "STOP", display list contents
		// and exit while loop 
		if (word == "STOP")
		{
			cout << "\nThe data you entered will now be displayed in reverse\n";
			cin.ignore();
			cin.get();
			list.display();
			break;
		}
		// otherwise, add the input to the linked list
		else
		{
			list.prepend(word);
		}	
	}

	// Exit program
	return 0;
}