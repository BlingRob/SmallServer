#include "client_request.h"
#include "parser.h"

#include <boost/bind.hpp>

ClientRequest::ClientRequest(boost::asio::io_service& ioService, const std::string& path) : clientSock_(ioService), rootPath_(path) { }

boost::asio::ip::tcp::socket& ClientRequest::Socket()
{
    return clientSock_;
}

void ClientRequest::Start()
{
    boost::asio::async_read_until(clientSock_, buf_, '\n', boost::bind(&ClientRequest::handleRead, this, _1, _2));
}

ClientRequest::~ClientRequest() 
{
    clientSock_.close();
}

void ClientRequest::handleRead(const boost::system::error_code& error,
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

void ClientRequest::handleWrite(const boost::system::error_code& error)
{
    delete this;
}