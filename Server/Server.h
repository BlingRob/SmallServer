#pragma once
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include "Headers.h"
#include "Parser.h"

class ClientRequest
{
	public:

		ClientRequest(boost::asio::io_service& io_service, const std::string& path) : clientSock_(io_service), rootPath_(path) { }

		boost::asio::ip::tcp::socket& Socket() 
		{
			return clientSock_;
		}

		void Start()
		{
			boost::asio::async_read_until(clientSock_, buf_, '\n', boost::bind(&ClientRequest::handleRead, this, _1, _2));
		}

		~ClientRequest() 
		{
			clientSock_.close();
		}

	private:

		void handleRead(const boost::system::error_code& error,
						size_t bytes_transferred)
		{
			if (!error)
			{
				Parser prs;
				std::istream in(&buf_);
				std::string msg;
				std::getline(in, msg);
				msg = prs.Get(msg, rootPath_);

				clientSock_.async_write_some(boost::asio::buffer(msg.c_str(), msg.size()), boost::bind(&ClientRequest::handleWrite, this,
					boost::asio::placeholders::error));
			}
			else
			{
				delete this;
			}
		}

		void handleWrite(const boost::system::error_code& error)
		{
			delete this;
		}

	private:

		boost::asio::ip::tcp::socket clientSock_;

		boost::asio::streambuf buf_;
		std::string rootPath_;
};

class Server
{
	public:

		Server(std::unique_ptr<ServerOptions>);
		void Run();

	private:

		void startAccept(ClientRequest*);
		void handleAccept(ClientRequest* req, const boost::system::error_code& err);
	
	private:

		typedef std::shared_ptr<ClientRequest> requestPtr_;
		std::unique_ptr<ServerOptions> info_;
		boost::asio::ip::tcp::endpoint point_;
		boost::asio::io_service service_;
		std::unique_ptr<boost::asio::ip::tcp::acceptor> acc_;
};
