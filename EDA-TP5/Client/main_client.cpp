#include <iostream>
#include "Client.h"

int main() {
	//Creates instance of client.
	Client myNewClient;

	//Begins connection to localhost.
	myNewClient.startConnection();

	//Prints received messages.
	myNewClient.printDialogue();

	return 0;
}