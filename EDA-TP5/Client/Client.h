#pragma once

#include <curl/curl.h>
#include <curl/easy.h>
#include <string>

#define HOST (const char*)"127.0.0.1/img"
//#define PATH (std::string)"/img"
#define PORT 80

class Client
{
public:
	Client(std::string host_ = HOST, std::string path_ = "lala", int port_ = PORT);

	void startConnection();

	void printDialogue(void);

	std::string& getBuffer(void);

	~Client();

private:

	void configurateClient(void);
	std::string path, host;
	int port;

	std::string message;
	CURL* handler;
	CURLcode error;
};
