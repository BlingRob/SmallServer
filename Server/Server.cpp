#include "Server.h"

Server::Server(std::unique_ptr<ServerOptions> data) 
{
	point_ = boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(data->IP), data->Port);
	acc_ = std::unique_ptr<boost::asio::ip::tcp::acceptor>(new boost::asio::ip::tcp::acceptor(service_, point_));
	info_ = std::move(data);
}

void Server::Run() 
{
	startAccept(new ClientRequest(service_, info_->Path));
	service_.run();
}

void Server::startAccept(ClientRequest* req)
{
	acc_->async_accept(req->Socket(), boost::bind(&Server::handleAccept, this, req, _1));
}

void Server::handleAccept(ClientRequest* req, const boost::system::error_code& err)
{
	if (!err)
		req->Start();
	else
		delete req;

	startAccept(new ClientRequest(service_, info_->Path));
	service_.run();
}
