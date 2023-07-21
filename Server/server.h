#pragma once
#include "client_request.h"
#include "server_configure.h"

#include <boost/asio.hpp>
#include <boost/thread.hpp>

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
