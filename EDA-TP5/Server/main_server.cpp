#include <iostream>
#include "Server.h"

int main() {
	boost::asio::io_context io_context;

	//Creates instance of server.
	Server myNewServer(io_context);

	//Runs server.
	for (;;)
		io_context.poll();

	return 0;
}