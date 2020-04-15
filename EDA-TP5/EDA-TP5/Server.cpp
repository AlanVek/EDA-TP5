#include "Server.h"

#include <iostream>
#include <boost\bind.hpp>

using boost::asio::ip::tcp;

#define PATH (std::string) "/img"

std::string make_daytime_string();

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
	if (socket.is_open()) {
		std::cout << "Error: Can't accept new connection from an open socket" << std::endl;
		return;
	}
	acceptor.async_accept(socket, boost::bind(&Server::connection_callback, this, boost::asio::placeholders::error));
}

void Server::act_upon_connection() {
	///*boost::asio::async_read(socket, message, boost::bind()

	//boost::asio::async_write(socket, boost::asio::buffer(message),
	//	boost::bind(&Server::sending_callback, this, boost::asio::placeholders::e*/rror, boost::asio::placeholders::bytes_transferred));

	socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
	socket.close();
}

void Server::input_validation(std::string& input) {
	std::string validator = "GET " + PATH + "  HTTP/1.1 \r\nHost: 127.0.0.1 \r\n";

	bool isInputOk = false;

	if (input.find(validator) == 0) {
		if (input[input.length() - 1] == '\r' && input[input.length()] == '\n' && input.length() > validator.length())
			isInputOk = true;
	}

	input_response(isInputOk);
}
void Server::connection_callback(const boost::system::error_code& error)
{
	if (!error) {
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

std::string make_daytime_string() {
#pragma warning(disable : 4996)
	using namespace std; // For time_t, time and ctime;
	time_t now = time(0);

	return ctime(&now);
}

void Server::input_response(bool isInputOk) {
}