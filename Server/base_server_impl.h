/// \file base_server_impl.h
/// \brief Base implementation of server

#pragma once
#ifndef BASE_SERVER_IMPL_H
#define BASE_SERVER_IMPL_H

#include "server_interface.h"
#include "server_parametrs.h"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/co_spawn.hpp>

class BaseServerImpl : IServer
{
    public:

    BaseServerImpl(const ServerParametrs& serverParametrs);

    void Start() override;

    void Stop() override;

    private:

    boost::asio::awaitable<void> listener();

    boost::asio::awaitable<void> reader(boost::asio::ip::tcp::socket socket);

    private:

    boost::asio::io_context ioContext_;

    ServerParametrs serverParametrs_;
};

#endif // BASE_SERVER_IMPL_H