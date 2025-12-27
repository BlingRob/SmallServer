#include "server.h"

#include <utils.h>

#include <boost/beast/version.hpp>
#include <boost/url.hpp>
#include <sstream>

namespace http = boost::beast::http;

BaseHttpServer::BaseHttpServer(boost::asio::io_context& ioc, const Endpoint& endpoint, ILogger& logger)
    : BaseTCPServer(ioc, endpoint, logger)
{
    logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);
}

std::string BaseHttpServer::user_handler(std::string request)
{
    logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);

    logger_.Log(std::format("Request: {}", request), Severities::Debug);

    http::request<http::string_body> req{string_to_request(std::move(request))};

    http::response<http::string_body> res{http_request_handler(std::move(req))};

    res.prepare_payload();

    std::string response_str{response_to_string(res)};

    logger_.Log(std::format("Response: {}", response_str), Severities::Debug);

    return response_str;
}

http::request<http::string_body> BaseHttpServer::string_to_request(std::string request)
{
    logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);

    boost::beast::error_code ec;
    auto buf{boost::asio::buffer(std::move(request))};
    http::request_parser<http::string_body> parser;
    parser.put(std::move(buf), ec);

    http::request<http::string_body> req{parser.release()};

    return req;
}

std::string BaseHttpServer::response_to_string(
    const boost::beast::http::response<boost::beast::http::string_body>& res) const
{
    logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);

    std::ostringstream ss;

    // Write status line
    ss << "HTTP/1.1 " << res.result_int() << " " << res.reason() << "\r\n";

    // Write headers
    for (auto const& field : res)
    {
        ss << field.name_string() << ": " << field.value() << "\r\n";
    }

    // Add empty line after headers
    ss << "\r\n";

    // Write body if present
    if (!res.body().empty())
    {
        ss << res.body().data() << "\r\n";
    }
    ss << "\r\n\r\n";

    return ss.str();
}
