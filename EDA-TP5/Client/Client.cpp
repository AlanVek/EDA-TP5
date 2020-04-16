#include "Client.h"
#include <iostream>

size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userData);

Client::Client(std::string host_, std::string path_, int port_) :

	host(host_), path(path_), port(port_)
{
	handler = nullptr;
	error = curl_global_init(CURL_GLOBAL_ALL);
	if (error == CURLE_OK) {
		handler = curl_easy_init();
		configurateClient();
	}
}

Client::~Client() {
	if (handler && error == CURLE_OK) {
		curl_easy_cleanup(handler);
		curl_global_cleanup();
	}
}

void Client::startConnection(void) {
	if (handler && error == CURLE_OK)
		error = curl_easy_perform(handler);
}

void Client::configurateClient(void) {
	if (handler && error == CURLE_OK) {
		curl_easy_setopt(handler, CURLOPT_URL, HOST);
		curl_easy_setopt(handler, CURLOPT_PORT, port);
		curl_easy_setopt(handler, CURLOPT_VERBOSE, 1L);
		curl_easy_setopt(handler, CURLOPT_PROTOCOLS, CURLPROTO_HTTP);
		curl_easy_setopt(handler, CURLOPT_WRITEFUNCTION, &write_callback);
		curl_easy_setopt(handler, CURLOPT_WRITEDATA, this);
	}
}

void Client::printDialogue(void) {
	if (handler && error == CURLE_OK) {
		std::cout << "The communication was:\n\n";

		std::cout << message << std::endl;
	}
	else
		std::cout << error;
}

size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userData) {
	std::cout << "Connection received. Got: " << size * nmemb << "bytes.\n";

	Client* userDataPtr = (Client*)userData;

	userDataPtr->getBuffer().append(ptr);

	return size * nmemb;
}

std::string& Client::getBuffer(void) { return message; }