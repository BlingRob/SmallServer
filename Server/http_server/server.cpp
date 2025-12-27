#include "server.h"

#include <utils.h>

#include <boost/beast/version.hpp>
#include <boost/json.hpp>
#include <boost/url.hpp>
#include <sstream>

namespace http = boost::beast::http;

SimpleHttpServer::SimpleHttpServer(boost::asio::io_context& ioc, const toml::table& cfg, ILogger& logger)
    : BaseHttpServer(ioc,
                     {cfg["connections"]["host"].value_or(default_configures::kHost),
                      cfg["connections"]["http_port"].value_or(default_configures::kHttpPort)},
                     logger)
{
    logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);

    auto host{cfg["connections"]["host"].value_or(default_configures::kHost)};
    auto port{cfg["connections"]["http_port"].value_or(default_configures::kHttpPort)};

    logger_.Log(std::format("Http Server {}:{} started", host, port), Severities::Info);
}

boost::beast::http::response<boost::beast::http::string_body> SimpleHttpServer::http_request_handler(
    boost::beast::http::request<boost::beast::http::string_body> request)
{
    logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);

    http::response<http::string_body> res;

    if (req.method() == http::verb::get)
    {
        // NOTE: For http://127.0.0.1:8080/api/login?username=myusername&password=mypassword
        // req.target() will be /api/login?username=myusername&password=mypassword
        boost::urls::url_view parsed_url(req.target());
        auto params{parsed_url.params()};

        // NOTE: Path is /api/login for the example above
        auto path{parsed_url.path()};

        if (path == "/hello")
        {
            boost::json::object json_response;
            json_response["message"] = "Hello, World!";
            json_response["status"] = "success";

            res.result(boost::beast::http::status::ok);
            res.set(boost::beast::http::field::content_type, "application/json");
            res.body() = boost::json::serialize(json_response);
        }
        else if (path == "/index.html")
        {
            res.version(11);
            res.result(http::status::ok);
            std::string file{utils::LoadTextFile("./index.html")};
            res.set(http::field::content_type, "text/html");
            res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
            // res.content_length(file.size());
            res.body() = std::move(file);
        }
        else if (path == "/world")
        {
            boost::json::object json_response;
            json_response["message"] = "World, Hello!";
            json_response["status"] = "success";

            res.result(boost::beast::http::status::ok);
            res.set(boost::beast::http::field::content_type, "application/json");
            res.body() = boost::json::serialize(json_response);
        }
        else
        {
            res.result(boost::beast::http::status::not_found);
            res.set(boost::beast::http::field::content_type, "text/plain");
            res.body() = "Not Found";
        }
    }
    else
    {
        res.result(boost::beast::http::status::method_not_allowed);
        res.set(boost::beast::http::field::content_type, "text/plain");
        res.body() = "Method Not Allowed";
    }

    return res;
}
