#pragma once

#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>
#include <format>
#include <memory>
#include <string>

#include "logger_subsystem/logger_interface.h"

using tcp_acceptor = boost::asio::use_awaitable_t<>::as_default_on_t<boost::asio::ip::tcp::acceptor>;
using tcp_socket = boost::asio::use_awaitable_t<>::as_default_on_t<boost::asio::ip::tcp::socket>;

struct Endpoint
{
    std::string ip;
    std::uint16_t port;
};

namespace this_coro = boost::asio::this_coro;

class BaseTCPServer : std::enable_shared_from_this<BaseTCPServer>
{
   public:
    BaseTCPServer(boost::asio::io_context& ioc, const Endpoint& endpoint, ILogger& logger)
        : logger_(logger), endpoint_(endpoint)
    {
        logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);

        boost::asio::co_spawn(ioc, listener(), boost::asio::detached);
    }

   protected:
    /// @brief Function for process external logic
    /// @param request string goten from client
    /// @return response result of client processing
    virtual std::string user_handler(std::string request) = 0;

    /// @brief Function for process request
    /// @param socket
    /// @return
    virtual boost::asio::awaitable<void> request_handler(tcp_socket socket)
    {
        logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);

        try
        {
            boost::asio::streambuf stream;
            std::istream is(&stream);
            std::ostream os(&stream);
            std::string str;
            /// @todo made different read politices
            // char data[1024];
            // boost::asio::dynamic_buffer(read_msg, 1024)
            // std::size_t n = co_await socket.async_read_some(boost::asio::buffer(data), boost::asio::use_awaitable);
            // co_await async_write(socket, boost::asio::buffer(data, n));

            std::size_t n{
                co_await boost::asio::async_read_until(socket, stream, "\r\n\r\n", boost::asio::use_awaitable)};
            str.resize(n);
            is.read(str.data(), n);
            stream.commit(n);
            stream.consume(n);
            str = co_await co_client_request(std::move(str));
            os.write(str.data(), str.size());

            co_await boost::asio::async_write(socket, stream, boost::asio::use_awaitable);
        }
        catch (const std::exception& e)
        {
            logger_.Log(std::format("Exception in request_handler: {}", e.what()), Severities::Error);
        }
    }

   protected:
    ILogger& logger_;

    Endpoint endpoint_;

   private:
    auto co_client_request(std::string&& msg, boost::asio::use_awaitable_t<> h = {})
    {
        logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);

        logger_.Log(std::format("co_client_request got {}", msg), Severities::Debug);

        auto initiate = [this](auto&& handler, std::string&& msg) mutable
        {
            auto ex{boost::asio::get_associated_executor(handler)};
            boost::asio::dispatch(ex,
                                  [this, handler = std::move(handler), msg = std::move(msg)]() mutable -> void
                                  { handler(user_handler(std::move(msg))); });
        };
        return boost::asio::async_initiate<boost::asio::use_awaitable_t<>, void(std::string&&)>(
            initiate, h, std::move(msg));
    }

    boost::asio::awaitable<void> listener()
    {
        logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);

        auto executor{co_await this_coro::executor};

        tcp_acceptor acceptor(executor, {boost::asio::ip::make_address_v4(endpoint_.ip), endpoint_.port});

        for (;;)
        {
            auto socket{co_await acceptor.async_accept()};
            boost::asio::co_spawn(executor, request_handler(std::move(socket)), boost::asio::detached);
        }
    }
};
