#pragma once

#include <boost/json.hpp>
#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <toml.hpp>

#include "base_http_server/server.h"

using response_t = boost::beast::http::response<boost::beast::http::string_body>;
namespace http = boost::beast::http;
namespace json = boost::json;

class WebGuiFtpServer : public BaseHttpServer
{
   public:
    WebGuiFtpServer(boost::asio::io_context& ioc, const toml::table& cfg, ILogger& logger);

   private:
    response_t http_request_handler(boost::beast::http::request<boost::beast::http::string_body>) override;

    response_t handle_get_files(const http::request<http::string_body>& req);

    response_t handle_upload(const http::request<http::string_body>& req);

    /// @todo realease this function
    // response_t handle_download(const http::request<http::string_body>& req);

    response_t handle_delete(const http::request<http::string_body>& req);

    response_t handle_mkdir(const http::request<http::string_body>& req);

    response_t handle_rmdir(const http::request<http::string_body>& req);

    response_t handle_rename(const http::request<http::string_body>& req);

    response_t serve_static_file(const http::request<http::string_body>& req);

    response_t html_response();

    response_t json_response(int status, const json::object& body);

    response_t cors_response();

    std::map<std::string, std::string> parse_query(const std::string& query);

    std::string url_decode(const std::string& str);

   private:
    std::string ftp_root_;

    std::string index_html_file_;
};