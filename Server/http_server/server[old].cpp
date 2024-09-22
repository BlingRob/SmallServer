#include "server.h"

#include <boost/bind.hpp>
#include <boost/asio/signal_set.hpp>

Server::Server(std::unique_ptr<ServerOptions> data) 
{
	point_ = boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(data->ip_), data->port_);
	acc_ = std::make_unique<boost::asio::ip::tcp::acceptor>(service_, point_);
	info_ = std::move(data);
}

void Server::Run() 
{
	startAccept(new ClientRequest(service_, info_->path_));

	// boost::asio::signal_set signals(service_, SIGINT, SIGTERM);
    // signals.async_wait([&](auto, auto)
	// { 
	// 	Stop();
	// });

	service_.run();
}

void Server::Stop()
{
	service_.stop();
}

void Server::startAccept(RequestPtr request)
{
	acc_->async_accept(request->Socket(), boost::bind(&Server::handleAccept, this, request, _1));
}

void Server::handleAccept(RequestPtr request, const boost::system::error_code& err)
{
	if (!err)
		request->Start();

	startAccept(new ClientRequest(service_, info_->path_));
	service_.run();
}
