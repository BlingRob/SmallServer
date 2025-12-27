#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <string>

#include "base_tcp_server/base_tcp_server.hpp"
#include "logger_subsystem/logger_interface.h"
#include "server_interface.h"

class BaseHttpServer : public BaseTCPServer
{
   public:
    BaseHttpServer(boost::asio::io_context& ioc, const Endpoint& endpoint, ILogger& logger);

   protected:
    virtual boost::beast::http::response<boost::beast::http::string_body> http_request_handler(
        boost::beast::http::request<boost::beast::http::string_body>) = 0;

   private:
    std::string user_handler(std::string request) override;

    boost::beast::http::request<boost::beast::http::string_body> string_to_request(std::string request);

    std::string response_to_string(const boost::beast::http::response<boost::beast::http::string_body>& res) const;
};
