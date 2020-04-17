#include <iostream>
#include "Client.h"

#define MAXSIZE 1000
bool getData(char* argV, int argC, std::string* host, std::string* path);

int main(int argC, char** argV) {
	std::string host, path;

	int result = -1;

	if (getData(argV[1], argC, &host, &path)) {
		result = 0;
		//Creates instance of client.
		Client myNewClient(path, host);

		//Begins connection to host.
		myNewClient.startConnection();

		//Saves eceived messages to file.
		myNewClient.saveDialogue();
	}
	else
		std::cout << "Failed to reach server. Wrong syntax.\n";

	return result;
}

/*Separates data into host, path and filename. Returns true if data is valid, otherwise it returns false.*/
bool getData(char* argV, int argC, std::string* host, std::string* path) {
	bool result = false;
	std::string data;
	int pos;

	//If the amount of arguments is correct (2)...
	if (argC == 2) {
		data = argV;
		result = true;

		//Searches for '/' symbol.
		pos = data.find('/');

		//If it found one, it separates in host and path.
		if (pos != std::string::npos) {
			*host = data.substr(0, pos);
			*path = data.substr(pos + 1, data.length() - pos + 1);
		}

		//Otherwise, it returns false.
		else
			result = false;
	}

	return result;
}