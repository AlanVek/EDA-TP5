#include <iostream>

#include "Server.h"
using namespace std;

int main() {
	boost::asio::io_context io_context;

	Server myNewServer(io_context);

	io_context.run();

	return 0;
}