#include <iostream>
//#include "Client.h"
#include "Server.h"

int main() {
	boost::asio::io_context io_context;

	Server myNewServer(io_context);

	//Client myNewClient;

	//myNewClient.startConnection();

	//myNewClient.printDialogue();

	for (;;)
		io_context.poll();

	return 0;
}