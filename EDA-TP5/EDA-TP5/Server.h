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

	//void input_validation(std::string& input);
	void input_validation(/*const boost::system::error_code& error, size_t bytes*/);

	void input_response(bool isInputOk);
	std::string generateTextResponse(bool);

	void connection_callback(const boost::system::error_code& error);
	void sending_callback(const boost::system::error_code& error, size_t bytes_sent);

	//int getFileLenght(std::fstream file);

	boost::asio::io_context& io_context;
	boost::asio::ip::tcp::acceptor acceptor;
	boost::asio::ip::tcp::socket socket;

	std::string message;
};
