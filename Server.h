#pragma once
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include "Headers.h"
#include "Parser.h"

class Client_Request
{
public:
	Client_Request(boost::asio::io_service& io_service,const std::string& path) :client_sock(io_service), RootPath(path){ }
	boost::asio::ip::tcp::socket& socket() {
		return client_sock;
	}
	void start()
	{
		boost::asio::async_read_until(client_sock, buf, '\n', boost::bind(&Client_Request::handle_read, this,_1, _2));
	}
	~Client_Request() 
	{
		client_sock.close();
	}
private:
	static const uint16_t BufSize = 255;
	boost::asio::ip::tcp::socket client_sock;

	char data[BufSize];
	boost::asio::streambuf buf;
	std::string RootPath;

	void handle_read(const boost::system::error_code& error,
		size_t bytes_transferred)
	{
		if (!error)
		{
			Parser prs;
			std::istream in(&buf);
			std::string msg;
			std::getline(in, msg);
			msg = prs.Get(msg, RootPath);
			client_sock.async_write_some(boost::asio::buffer(msg.c_str(),msg.size()), boost::bind(&Client_Request::handle_write, this,
				boost::asio::placeholders::error));
		}
		else
		{
			delete this;
		}
	}

	void handle_write(const boost::system::error_code& error)
	{
		delete this;
	}
};

class Server
{
public:
	Server(std::unique_ptr<server_options>);
	void run();
private:
	typedef std::shared_ptr<Client_Request> request_ptr;
	std::unique_ptr<server_options> info;
	boost::asio::ip::tcp::endpoint point;
	boost::asio::io_service service;
	std::unique_ptr<boost::asio::ip::tcp::acceptor> acc;

	void start_accept(Client_Request*);
	void handle_accept(Client_Request* req, const boost::system::error_code& err);
};
