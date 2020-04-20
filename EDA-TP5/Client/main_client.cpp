#include <iostream>
#include "Client.h"
#include "DataInput.h"

int main(int argC, char** argV) {
	Location userData;

	int result = -1;

	if (getData(argV, argC, &userData)) {
		result = 0;
		//Creates instance of client.
		Client myNewClient(userData.host, userData.path, 80);

		//Begins connection to host.
		myNewClient.startConnection();

		//Saves eceived messages to file.
		myNewClient.saveDialogue();
	}
	else
		std::cout << "Failed to reach server. Wrong syntax.\n";

	return result;
}