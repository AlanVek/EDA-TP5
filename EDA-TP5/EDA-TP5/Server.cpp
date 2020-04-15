#include "Server.h"

#include <iostream>
#include <boost\bind.hpp>
#include <chrono>
#include <fstream>

using boost::asio::ip::tcp;
#define HOST (std::string) "127.0.0.1"
#define HOST2 (std::string) "192.168.1.35"
#define PATH (std::string) "/img"
#define FILENAME  "page.html"

std::string make_daytime_string(bool plusThirty);

/*Server constructor. Initializes io_context, acceptor and socket.
Calls wait_for_connection to accept connections.*/
Server::Server(boost::asio::io_context& io_context_) :
	io_context(io_context_), acceptor(io_context_, tcp::endpoint(tcp::v4(), 80)), socket(io_context_)
{
	if (socket.is_open()) {
		socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
		socket.close();
	}
	wait_for_connection();
}

//Destructor. Closes open socket and acceptor.
Server::~Server() {
	if (socket.is_open()) {
		socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
		socket.close();
	}

	if (acceptor.is_open())
		acceptor.close();
}

/*Sets acceptor to accept (asynchronously).*/
void Server::wait_for_connection() {
	std::cout << "Waiting for connection.\n";

	if (socket.is_open()) {
		std::cout << "Error: Can't accept new connection from an open socket" << std::endl;
		return;
	}
	acceptor.async_accept(socket, boost::bind(&Server::connection_callback, this, boost::asio::placeholders::error));
}

//Closes socket.
void Server::closeConnection() {
	std::cout << "Closing socket.\n";

	socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
	socket.close();
}

/*Validates input given in GET request.*/
void Server::input_validation(const boost::system::error_code& error, size_t bytes) {
	if (!error) {
		//Creates string message from request.
		std::string message(mess);

		//Validator has the http protocol form.
		std::string validator = "GET " + PATH + " HTTP/1.1\r\nHost: " + HOST + "\r\n";
		bool isInputOk = false;

		//If there's been a match at the beggining of the request...
		if (message.find(validator) == 0) {
			//If the request has CRLF at end, then input was valid.
			if (message.length() > validator.length()
				&& message[message.length() - 2] == '\r'
				&& message[message.length() - 1] == '\n')

				isInputOk = true;
		}

		//Generates response (according to validity of input).
		input_response(isInputOk);
	}

	//If there's been an error, prints the message.
	else
		std::cout << error.message() << std::endl;
}

/*Called when there's been a connection.*/
void Server::connection_callback(const boost::system::error_code& error) {
	if (!error)

		//Sets socket to read request.
		socket.async_read_some
		(
			boost::asio::buffer(mess, MAXSIZE),
			boost::bind
			(
				&Server::input_validation,
				this, boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred
			)
		);

	else
		std::cout << error.message() << std::endl;
}
/*Called when a message has been sent to client.*/
void Server::sending_callback(const boost::system::error_code& error, size_t bytes_sent)
{
	if (!error)
		std::cout << "Response sent. " << bytes_sent << " bytes." << std::endl;

	else
		std::cout << "Failed to respont to connection" << std::endl;
}

/*Returns daytime string. If plusThirty is true, it returns
current daytime + 30 seconds.*/
std::string make_daytime_string(bool plusThirty) {
	using namespace std::chrono;
	system_clock::time_point theTime = system_clock::now();

	if (plusThirty)
		theTime += seconds(30);

	time_t now = system_clock::to_time_t(theTime);

	return ctime(&now);
}

/*Responds to input.*/
void Server::input_response(bool isInputOk) {
	//Opens file.
	std::fstream page(FILENAME, std::ios::in | std::ios::binary);

	/*Checks if file was correctly open.*/
	if (!page.is_open())
		return;

	size = getFileLength(page);

	/*Generates text response, according to validity of input.*/
	response = generateTextResponse(isInputOk);

	/*If input was correct, appends text from file.*/
	if (isInputOk) {
		std::ostringstream ss;
		ss << page.rdbuf();
		response += ss.str();
	}
	response += "\r\n\r\n";

	/*Sets socket to write (send to client).*/
	socket.async_write_some
	(
		boost::asio::buffer(response),
		boost::bind
		(
			&Server::sending_callback,
			this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred
		)
	);

	/*Closes connection and frees acceptor again.*/
	closeConnection();
	wait_for_connection();

	//Closes file.
	page.close();
}

/*Generates http response, according to validity of input.*/
std::string Server::generateTextResponse(bool isInputOk) {
	std::string date = make_daytime_string(false);
	std::string datePlusThirty = make_daytime_string(true);
	std::string response;
	if (isInputOk) {
		response =
			"HTTP/1.1 200 OK\r\nDate:" + date + "Location: " + HOST +
			PATH + "\r\nCache-Control: max-age=30\r\nExpires:" +
			datePlusThirty +
			"Content-Length:" + std::to_string(size) +
			"\r\nContent-Type: text/html; charset=iso-8859-1\r\n\r\n";
	}
	else {
		response =
			"HTTP/1.1 404 Not Found\r\nDate:" + date + "Location: " + HOST +
			PATH + "\r\nCache-Control: public, max-age=30 \r\nExpires:" + datePlusThirty + "Content-Length: 0" +
			" \r\nContent-Type: text/html; charset=iso-8859-1\r\n\r\n";
	}

	return response;
}
/*Gets file length.*/
size_t Server::getFileLength(std::fstream& file) {
	file.seekg(0, file.end);

	size_t length = file.tellg();

	file.seekg(0, file.beg);
	return length;
}