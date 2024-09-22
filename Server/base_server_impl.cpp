/// \file base_server_impl.cpp

#include "base_server_impl.h"

#include <boost/asio/detached.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/buffer.hpp>

#include <string>

BaseServerImpl::BaseServerImpl(const ServerParametrs& serverParametrs):
                ioContext_(1),
                serverParametrs_(serverParametrs)
{
    co_spawn(ioContext_, listener(), boost::asio::detached);
}

void BaseServerImpl::Start()
{
    ioContext_.run();
}

void BaseServerImpl::Stop()
{
    ioContext_.stop();
}

boost::asio::awaitable<void> BaseServerImpl::reader(boost::asio::ip::tcp::socket socket)
{
  try
  {
    char data[1024];
    for (std::string read_msg;;)
    {
        //std::size_t n = co_await socket.async_read_some(boost::asio::buffer(data), boost::asio::use_awaitable);
        std::size_t n = co_await boost::asio::async_read_until(socket,
            boost::asio::dynamic_buffer(read_msg, 1024), "\n", boost::asio::use_awaitable);
        // std::printf("Read bytes: %d\n", n);
        // std::printf("Read: %s\n", read_msg.c_str());
        // if(read_msg.find("HTTP"))
        // {
        //     //n = co_await socket.async_read_some(boost::asio::buffer(data), boost::asio::use_awaitable);
        //     std::string answer{"HTTP/1.1 308 Permanent Redirect\n\r Location: 127.0.0.1:15000\n\r"};
        //     co_await boost::asio::async_write(socket, boost::asio::buffer(answer, answer.size()), boost::asio::use_awaitable);
        //     break;
        // }
        read_msg.erase(0, n);
        // std::size_t n = co_await boost::asio::async_read_until(socket, boost::asio::buffer(data), '\n',  boost::asio::use_awaitable);
        //co_await boost::asio::async_write(socket, boost::asio::buffer(read_msg, n), boost::asio::use_awaitable);
    }
  }
  catch (std::exception& e)
  {
    std::printf("echo Exception: %s\n", e.what());
  }
}

boost::asio::awaitable<void> BaseServerImpl::listener()
{
  auto executor = co_await boost::asio::this_coro::executor;
  boost::asio::ip::tcp::acceptor acceptor(executor, boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(serverParametrs_.ip_), serverParametrs_.port_));
  for (;;)
  {
    boost::asio::ip::tcp::socket socket = co_await acceptor.async_accept(boost::asio::use_awaitable);
    co_spawn(executor, reader(std::move(socket)), boost::asio::detached);
  }
}