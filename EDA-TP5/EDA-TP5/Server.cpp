#include "Server.h"

#include <iostream>
#include <boost\bind.hpp>
#include <chrono>
#include <fstream>

using boost::asio::ip::tcp;

#define PATH (std::string) "/img"

std::string make_daytime_string(bool plusThirty);

Server::Server(boost::asio::io_context& io_context_) : io_context(io_context_), acceptor(io_context_, tcp::endpoint(tcp::v4(), 80)), socket(io_context_)
{
	if (socket.is_open()) {
		socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
		socket.close();
	}
	wait_for_connection();
}

Server::~Server() {
	if (socket.is_open()) {
		socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
		socket.close();
	}

	if (acceptor.is_open())
		acceptor.close();
}

void Server::wait_for_connection()
{
	std::cout << "Waiting for connection.\n";

	if (socket.is_open()) {
		std::cout << "Error: Can't accept new connection from an open socket" << std::endl;
		return;
	}
	acceptor.async_accept(socket, boost::bind(&Server::connection_callback, this, boost::asio::placeholders::error));
	//socket.async_receive(boost::asio::buffer(message), boost::bind(&Server::input_validation, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void Server::act_upon_connection() {
	std::cout << "Closing socket.\n";

	socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
	socket.close();
}

void Server::input_validation(/*const boost::system::error_code& error, size_t bytes*/) {
	//if (!error) {
	std::cout << "Validating input" << std::endl;
	std::string validator = "GET " + PATH + "  HTTP/1.1 \r\nHost: 127.0.0.1 \r\n";
	std::cout << "Validator created\n";
	bool isInputOk = false;

	std::cout << message << std::endl;
	if (message.find(validator) == 0) {
		if (message[message.length() - 1] == '\r' && message[message.length()] == '\n' && message.length() > validator.length())
			isInputOk = true;
		//}

		std::cout << "Message run. Calling input response.\n";

		input_response(isInputOk);
	}

	/*else
		std::cout << error.message() << std::endl;*/
}
void Server::connection_callback(const boost::system::error_code& error)
{
	if (!error) {
		socket.receive(boost::asio::buffer(message));
		input_response(true);

		std::cout << "This is the message: " + message << std::endl;
		act_upon_connection();
		wait_for_connection();
	}
	else
		std::cout << error.message() << std::endl;
}

void Server::sending_callback(const boost::system::error_code& error, size_t bytes_sent)
{
	if (!error)
		std::cout << "Response sent. " << bytes_sent << " bytes." << std::endl;

	else
		std::cout << "Failed to respont to connection" << std::endl;
}

//std::string make_daytime_string(bool plusThirty) {
//	std::chrono::system_clock::time_point theTime = std::chrono::system_clock::now();
//
//	if (plusThirty)
//		theTime += std::chrono::seconds(30);
//
//	time_t now = std::chrono::system_clock::to_time_t(theTime);
//
//	return ctime(&now);
//}
std::string make_daytime_string(bool plusThirty)
{
#pragma warning(disable : 4996)
	using namespace std; // For time_t, time and ctime;
	time_t now = time(0);
	return ctime(&now);
}

void Server::input_response(bool isInputOk) {
	std::cout << "Inside input response\n";
	std::cout << isInputOk << std::endl;
	std::fstream page("pag.html");
	if (!page.is_open())
		return;

	std::string response = generateTextResponse(isInputOk);
	std::cout << response << std::endl;

	std::cout << "Async writing\n";

	socket.send(boost::asio::buffer(response));
	/*boost::asio::async_write(
		socket,
		boost::asio::buffer(response),
		boost::bind(&Server::sending_callback, this, boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred)*/
			//);

	std::cout << "Checking for file\n";

	if (isInputOk) {
		std::ostringstream ss;
		ss << page.rdbuf();
		response = ss.str();

		std::cout << "Async writing file.\n";
		/*boost::asio::async_write(
			socket,
			boost::asio::buffer(response),
			boost::bind(&Server::sending_callback, this, boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred)
		);*/
		socket.send(boost::asio::buffer(response));
	}
	response = "\r\n";
	std::cout << "Writing end of statement.\n";

	/*boost::asio::async_write(
		socket,
		boost::asio::buffer(response),
		boost::bind(&Server::sending_callback, this, boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));*/
	socket.send(boost::asio::buffer(response));
}

std::string Server::generateTextResponse(bool isInputOk) {
	std::cout << "Generating text response\n";
	std::string date = make_daytime_string(false);
	std::string datePlusThirty = make_daytime_string(true);
	std::string response;
	if (isInputOk) {
		response =
			"HTTP/1.1 200 OK \r\nDate:" + date + " \r\nLocation: 127.0.0.1" +
			PATH + " \r\nCache-Control: max-age=30 \r\nExpires:" +
			datePlusThirty +
			" \r\nContent-Length:" +
			/**/
			" \r\nContent-Type: text/html; charset=iso-8859-1 \r\n";
	}
	else {
		response =
			"HTTP/1.1 200 404 Not Found \r\nDate:" + date + "Location: 127.0.0.1" +
			PATH + "\r\nCache-Control: public, max-age=30 \r\nExpires:" + datePlusThirty + "Content-Length: 0" +
			" \r\nContent-Type: text/html; charset=iso-8859-1\r\n";
	}

	std::cout << "Text response generated.\n";
	return response;
}