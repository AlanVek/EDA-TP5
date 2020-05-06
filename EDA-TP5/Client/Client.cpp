#include "Client.h"
#include <iostream>
#include <curl/easy.h>
#include <fstream>
#include "Errors.h"

//Callback for when message is received.
size_t writeCallback(char* ptr, size_t size, size_t nmemb, void* userData);

//Client constructor. Initializes CURL, and the easy mode. Calls client configuration.
Client::Client(std::string host_, std::string path_, int port_) : host(host_), path(path_), port(port_)
{
	handler = nullptr;

	error = curl_global_init(CURL_GLOBAL_ALL);

	if (error == CURLE_OK) {
		handler = curl_easy_init();
		configurateClient();
	}
	else
		throw(Error("Failed to set Curl.\n"));
}

//Client destructor.
Client::~Client() {
	if (handler) {
		curl_easy_cleanup(handler);
		curl_global_cleanup();
	}
	message.close();
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

//Write callback. Appends incoming message to this->message.
size_t writeCallback(char* ptr, size_t size, size_t nmemb, void* userData) {
	Client* userDataPtr = (Client*)userData;

	std::fstream& output = userDataPtr->getBuffer();

	if (!output.is_open())
		userDataPtr->openFile();

	output.write(ptr, nmemb);

	return size * nmemb;
}

//Message getter.
std::fstream& Client::getBuffer(void) { return message; }

void Client::openFile(void) {
	std::string filename;
	std::string tempfile;
	int pos = path.find_last_of('.');

	auto removeBar = [](std::string& name) {std::string temp; for (auto x : name) { if (x != '/') temp += x; } return temp; };

	error = curl_easy_getinfo(handler, CURLINFO_CONTENT_TYPE, &contentType);

	if (error == CURLE_OK) {
		if (pos == std::string::npos || pos == path.length()) {
			filename = removeBar(host);
			filename = filename.substr(filename.length() - 10, 10);
		}
		else {
			filename = path.substr(0, pos);
			filename = removeBar(filename);
			if (filename.length() > 10)
				filename = filename.substr(filename.length() - 10, 10);
		}

		std::string ctnt = contentType;
		pos = ctnt.find('/');
		ctnt = ctnt.substr(pos + 1, ctnt.length() - pos);

		message.open((filename + '.' + ctnt).c_str(), std::ios::out | std::ios::binary);
		if (!message.is_open()) {
			message.close();
			throw(Error("Failed to create output file.\n"));
		}
	}
	else
		throw(Error("Failed to get content type.\n"));
}