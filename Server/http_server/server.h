#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <string>
#include <toml.hpp>

#include "base_http_server/server.h"
#include "logger_subsystem/logger_interface.h"
#include "server_interface.h"

class SimpleHttpServer : public BaseHttpServer
{
   public:
    SimpleHttpServer(boost::asio::io_context& ioc, const toml::table& cfg, ILogger& logger);

   private:
    boost::beast::http::response<boost::beast::http::string_body> http_request_handler(
        boost::beast::http::request<boost::beast::http::string_body>) override;
};
