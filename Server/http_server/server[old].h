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

		void Stop();

	private:

		//typedef std::shared_ptr<ClientRequest> RequestPtr;
		typedef ClientRequest* RequestPtr;

		void startAccept(RequestPtr request);
		void handleAccept(RequestPtr request, const boost::system::error_code& err);
	
	private:
		std::unique_ptr<ServerOptions> info_;
		boost::asio::ip::tcp::endpoint point_;
		boost::asio::io_context service_;
		std::unique_ptr<boost::asio::ip::tcp::acceptor> acc_;
};
