#pragma once

#include <curl/curl.h>
#include <string>

#define HOST (std::string)"127.0.0.1"
#define PATH (std::string) "/img/page.html"

class Client
{
public:
	Client(std::string path_ = PATH, std::string host_ = HOST);

	void startConnection();

	void saveDialogue(void);

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
