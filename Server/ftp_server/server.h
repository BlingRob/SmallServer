#pragma once

#include <string>
#include <toml.hpp>

#include "base_tcp_server/base_tcp_server.hpp"
#include "session.h"

class FTPServer : public BaseTCPServer
{
   public:
    FTPServer(boost::asio::io_context& ioc, const toml::table& cfg, ILogger& logger);

   private:
    /// @todo Make something with server
    std::string user_handler(std::string request) override { return std::string{}; };
    boost::asio::awaitable<void> request_handler(tcp_socket socket) override;

   private:
    std::string root_directory_;
};