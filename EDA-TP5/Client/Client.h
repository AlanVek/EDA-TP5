#pragma once

#include <curl/curl.h>
#include <string>

#define HOST (std::string)"127.0.0.1"
#define PATH (std::string) "/img/page/page.html"
#define PORT 80

class Client
{
public:
	Client(std::string host_ = HOST, std::string path_ = PATH, int port_ = PORT);

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
