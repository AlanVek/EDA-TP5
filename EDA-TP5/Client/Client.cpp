#include "Client.h"
#include <iostream>
#include <string>
#include <curl/easy.h>
#include <fstream>

#define PORT 80

//Callback for when message is received.
size_t writeCallback(char* ptr, size_t size, size_t nmemb, void* userData);

//Client constructor. Initializes CURL, and the easy mode. Calls client configuration.
Client::Client(std::string path_, std::string host_) :

	path(path_), host(host_)
{
	port = PORT;
	handler = nullptr;

	error = curl_global_init(CURL_GLOBAL_ALL);

	if (error == CURLE_OK) {
		handler = curl_easy_init();
		configurateClient();
	}
}

//Client destructor.
Client::~Client() {
	if (handler && error == CURLE_OK) {
		curl_easy_cleanup(handler);
		curl_global_cleanup();
	}
}

//After initial setup, activates the handler.
void Client::startConnection(void) {
	if (handler && error == CURLE_OK)
		error = curl_easy_perform(handler);
}

/*Configurates client to send a GET request to server and
write the data gathered with write_callback into "this" client as userData.*/
void Client::configurateClient(void) {
	if (handler && error == CURLE_OK) {
		curl_easy_setopt(handler, CURLOPT_URL, (host + '/' + path).c_str());
		curl_easy_setopt(handler, CURLOPT_PORT, port);
		curl_easy_setopt(handler, CURLOPT_PROTOCOLS, CURLPROTO_HTTP);
		curl_easy_setopt(handler, CURLOPT_WRITEFUNCTION, &writeCallback);
		curl_easy_setopt(handler, CURLOPT_WRITEDATA, this);
	}
}

//Prints received messages.
void Client::saveDialogue(void) {
	if (handler && error == CURLE_OK) {
		if (message.length() > 0) {
			std::fstream myFile;
			myFile.open("result.bin", std::ios::out | std::ios::binary);
			if (!myFile.is_open()) {
				std::cout << "Failed to create output file.\n";
				myFile.close();
				return;
			}
			myFile.write(message.c_str(), message.length() * sizeof(char));
			myFile.close();
			std::cout << "Message received correctly. Got " << message.size() << " bytes.\n";
		}
		else
			std::cout << "Got 404 error.\n";
	}
	else
		std::cout << error;
}

//Write callback. Appends incoming message to this->message.
size_t writeCallback(char* ptr, size_t size, size_t nmemb, void* userData) {
	Client* userDataPtr = (Client*)userData;

	userDataPtr->getBuffer().append(ptr);

	return size * nmemb;
}

//Message getter.
std::string& Client::getBuffer(void) { return message; }