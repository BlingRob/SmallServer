#pragma once

#include <logger_subsystem/logger.h>
#include <server_parameters.h>

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>

#include <cstdio>
#include <string>
#include <functional>
#include <memory>

using tcp_acceptor = boost::asio::use_awaitable_t<>::as_default_on_t<boost::asio::ip::tcp::acceptor>;
using tcp_socket = boost::asio::use_awaitable_t<>::as_default_on_t<boost::asio::ip::tcp::socket>;

namespace this_coro = boost::asio::this_coro;

class BaseTCPServer: std::enable_shared_from_this<BaseTCPServer>
{
    public:

    BaseTCPServer(const ServerParameters& parameters, boost::asio::io_context& ioc)
          : serv_parameters_(parameters)
    {
        boost::asio::co_spawn(ioc, listener(), boost::asio::detached);
    }

    protected:

    /// @brief Function for process external logic
    /// @param request string goten from client
    /// @return response result of client processing
    virtual std::string user_handler(std::string request) = 0;

    protected:

    ServerParameters serv_parameters_;

    private:

    auto co_client_request(std::string&& msg, boost::asio::use_awaitable_t<> h = {})
    {   
        auto initiate = [this](auto&& handler, std::string&& msg) mutable 
        {
            auto ex = boost::asio::get_associated_executor(handler);
            boost::asio::dispatch(ex, [this, handler = std::move(handler), msg = std::move(msg)] () mutable -> void
            {
                handler(user_handler(std::move(msg)));
            });
        };
        return boost::asio::async_initiate<boost::asio::use_awaitable_t<>, void(std::string&&)>(initiate, h, std::move(msg));
    }

    boost::asio::awaitable<void> request_handler(tcp_socket socket)
    {
        try
        {
            boost::asio::streambuf stream;
            std::istream is(&stream);
            std::ostream os(&stream);
            std::string str;
            /// @todo made different read politices
            //   std::size_t n = co_await socket.async_read_some(boost::asio::buffer(data));
            //   co_await async_write(socket, boost::asio::buffer(data, n));

            // char data[1024];
            // for (std::string read_msg;;)
            // {
            //     //std::size_t n = co_await socket.async_read_some(boost::asio::buffer(data), boost::asio::use_awaitable);
            //     std::size_t n = co_await boost::asio::async_read_until(socket,
            //         boost::asio::dynamic_buffer(read_msg, 1024), "\n", boost::asio::use_awaitable);
            //     // std::printf("Read bytes: %d\n", n);
            //     // std::printf("Read: %s\n", read_msg.c_str());
            //     // if(read_msg.find("HTTP"))
            //     // {
            //     //     //n = co_await socket.async_read_some(boost::asio::buffer(data), boost::asio::use_awaitable);
            //     //     
            //     //     co_await boost::asio::async_write(socket, boost::asio::buffer(answer, answer.size()), boost::asio::use_awaitable);
            //     //     break;
            //     // }
            //     read_msg.erase(0, n);
            //     // std::size_t n = co_await boost::asio::async_read_until(socket, boost::asio::buffer(data), '\n',  boost::asio::use_awaitable);
            //     //co_await boost::asio::async_write(socket, boost::asio::buffer(read_msg, n), boost::asio::use_awaitable);
            // }

            std::size_t n = co_await boost::asio::async_read_until(socket, stream, "\r\n\r\n", boost::asio::use_awaitable);
            str.resize(n);
            is.read(str.data(), n);
            stream.commit(n);
            str = co_await co_client_request(std::move(str));
            // os << co_await co_client_request(std::move(str));
            os.write(str.data(), str.size());
            
            co_await async_write(socket, stream);
        }
        catch (const std::exception& e)
        {
            /// @todo need logging
            // std::printf("echo Exception: %s\n", e.what());
        }
    }

    boost::asio::awaitable<void> listener()
    {
        auto executor = co_await this_coro::executor;

        tcp_acceptor acceptor(executor, {boost::asio::ip::make_address_v4(serv_parameters_.host_), serv_parameters_.port_});
        
        for (;;)
        {
            auto socket = co_await acceptor.async_accept();
            boost::asio::co_spawn(executor, request_handler(std::move(socket)), boost::asio::detached);
        }
    }
};
