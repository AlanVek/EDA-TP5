#include "Client.h"
#include <iostream>
#include <string>

#define MAXSIZE 100;
//Callback for when message is received.
size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userData);

//Client constructor. Initializes CURL, and the easy mode. Calls client configuration.
Client::Client(std::string path_, std::string host_, int port_) :

	path(path_), host(host_), port(port_)
{
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
		curl_easy_setopt(handler, CURLOPT_URL, (host + path).c_str());
		curl_easy_setopt(handler, CURLOPT_PORT, port);
		curl_easy_setopt(handler, CURLOPT_VERBOSE, 1L);
		curl_easy_setopt(handler, CURLOPT_PROTOCOLS, CURLPROTO_HTTP);
		curl_easy_setopt(handler, CURLOPT_WRITEFUNCTION, &write_callback);
		curl_easy_setopt(handler, CURLOPT_WRITEDATA, this);
	}
}

//Prints received messages.
void Client::printDialogue(void) {
	if (handler && error == CURLE_OK) {
		std::cout << "The communication was:\n\n";

		std::cout << message << std::endl;
	}
	else
		std::cout << error;
}

//Write callback. Appends incoming message to this->message.
size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userData) {
	std::cout << "Connection received. Got: " << size * nmemb << "bytes.\n";

	Client* userDataPtr = (Client*)userData;

	userDataPtr->getBuffer().append(ptr);

	return size * nmemb;
}

//Message getter.
std::string& Client::getBuffer(void) { return message; }