#include "Server.h"

#include <iostream>
#include <boost\bind.hpp>
#include <boost/any.hpp>
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
}

void Server::act_upon_connection() {
	std::cout << "Closing socket.\n";

	socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
	socket.close();
}

void Server::input_validation(const boost::system::error_code& error, size_t bytes) {
	if (!error) {
		std::string message(mess);
		std::string validator = "GET " + PATH + " HTTP/1.1\r\nHost: 127.0.0.1\r\n";
		bool isInputOk = false;

		if (message.find(validator) == 0) {
			if (message.length() > validator.length() && message[message.length() - 2] == '\r' && message[message.length() - 1] == '\n')
				isInputOk = true;
		}

		input_response(isInputOk);
	}
	else
		std::cout << error.message() << std::endl;
}
void Server::connection_callback(const boost::system::error_code& error)
{
	if (!error)
		socket.async_receive
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

void Server::sending_callback(const boost::system::error_code& error, size_t bytes_sent)
{
	if (!error)
		std::cout << "Response sent. " << bytes_sent << " bytes." << std::endl;

	else
		std::cout << "Failed to respont to connection" << std::endl;
}

std::string make_daytime_string(bool plusThirty) {
	std::chrono::system_clock::time_point theTime = std::chrono::system_clock::now();

	if (plusThirty)
		theTime += std::chrono::seconds(30);

	time_t now = std::chrono::system_clock::to_time_t(theTime);

	return ctime(&now);
}

void Server::input_response(bool isInputOk) {
	std::fstream page("pag.html");
	if (!page.is_open())
		return;

	size = getFileLength(page);
	response = generateTextResponse(isInputOk);

	//socket.send(boost::asio::buffer(response));

	if (isInputOk) {
		std::ostringstream ss;
		ss << page.rdbuf();
		response += ss.str();

		//socket.send(boost::asio::buffer(response));
	}
	response += "\r\n";

	boost::asio::async_write(
		socket,
		boost::asio::buffer(response),
		boost::bind(&Server::sending_callback, this, boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	//socket.send(boost::asio::buffer(response));
	act_upon_connection();
	wait_for_connection();
}

std::string Server::generateTextResponse(bool isInputOk) {
	std::string date = make_daytime_string(false);
	std::string datePlusThirty = make_daytime_string(true);
	std::string response;
	if (isInputOk) {
		response =
			"HTTP/1.1 200 OK\r\nDate:" + date + "\r\nLocation: 127.0.0.1" +
			PATH + "\r\nCache-Control: max-age=30\r\nExpires:" +
			datePlusThirty +
			"\r\nContent-Length:" + std::to_string(size) +
			"\r\nContent-Type: text/html; charset=iso-8859-1\r\n";
	}
	else {
		response =
			"HTTP/1.1 404 Not Found\r\nDate:" + date + "Location: 127.0.0.1" +
			PATH + "\r\nCache-Control: public, max-age=30 \r\nExpires:" + datePlusThirty + "Content-Length: 0" +
			" \r\nContent-Type: text/html; charset=iso-8859-1\r\n";
	}

	return response;
}

size_t Server::getFileLength(std::fstream& file) {
	file.seekg(0, file.end);

	size_t length = file.tellg();

	file.seekg(0, file.beg);
	return length;
}