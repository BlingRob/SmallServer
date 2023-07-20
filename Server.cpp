#include "Server.h"

Server::Server(std::unique_ptr<server_options> data) 
{
	point = boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(data->IP), data->Port);
	acc = std::unique_ptr<boost::asio::ip::tcp::acceptor>(new boost::asio::ip::tcp::acceptor(service, point));
	info = std::move(data);
}

void Server::run() 
{
	start_accept(new Client_Request(service,info->Path));
	service.run();
}

void Server::start_accept(Client_Request* req)
{
	acc->async_accept(req->socket(), boost::bind(&Server::handle_accept, this, req, _1));
}
void Server::handle_accept(Client_Request* req, const boost::system::error_code& err)
{
	if (!err)
		req->start();
	else
		delete req;
	start_accept(new Client_Request(service, info->Path));
	service.run();
}
