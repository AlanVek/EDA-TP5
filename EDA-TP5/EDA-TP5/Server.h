#pragma once

#include <boost/asio.hpp>
#include <string>

class Server
{
public:
	Server(boost::asio::io_context& io_context_);

	~Server();
private:
	void wait_for_connection(void);
	void act_upon_connection(void);

	void input_validation(std::string& input);

	void input_response(bool isInputOk);

	void connection_callback(const boost::system::error_code& error);
	void sending_callback(const boost::system::error_code& error, size_t bytes_sent);

	boost::asio::io_context& io_context;
	boost::asio::ip::tcp::acceptor acceptor;
	boost::asio::ip::tcp::socket socket;

	std::string message;
};
