#include "server.h"
#include <utils.h>

#include <iostream>
#include <sstream>

#include <boost/beast/version.hpp>
#include <boost/json.hpp>
#include <boost/url.hpp>

namespace http = boost::beast::http;

HttpServer::HttpServer(const ServerParameters& parameters, boost::asio::io_context& ioc)
					  : server_parameters_{parameters}
					  , BaseTCPServer(parameters, ioc)
{

}

std::string HttpServer::user_handler(std::string request)
{
	std::cout << "Request: " << request << std::endl;

	http::request<http::string_body> req{string_to_request(std::move(request))};

	// Prepare the response
	http::response<http::string_body> res;
	// res.result(http::status::ok);
	// res.set(http::field::content_length, "Hello");
	// res.prepare_payload();

    if (req.method() == http::verb::get) {
        // NOTE: For http://127.0.0.1:8080/api/login?username=myusername&password=mypassword
        // req.target() will be /api/login?username=myusername&password=mypassword
        boost::urls::url_view parsed_url(req.target());
        auto params = parsed_url.params();

        // NOTE: Path is /api/login for the example above
        auto path = parsed_url.path();

        if(path == "/hello") {
            boost::json::object json_response;
            json_response["message"] = "Hello, World!";
            json_response["status"] = "success";

            res.result(boost::beast::http::status::ok);
            res.set(boost::beast::http::field::content_type, "application/json");
            res.body() = boost::json::serialize(json_response);
            res.prepare_payload();
        } else if(path == "/index.html")
        {
            res.version(11);
            res.result(http::status::ok);
            std::string file = LoadTextFile("./index.html");
            res.set(http::field::content_type, "text/html");
            res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
            // res.content_length(file.size());
            res.body() = std::move(file);
            res.prepare_payload();
        } else if(path == "/world") {
            boost::json::object json_response;
            json_response["message"] = "World, Hello!";
            json_response["status"] = "success";

            res.result(boost::beast::http::status::ok);
            res.set(boost::beast::http::field::content_type, "application/json");
            res.body() = boost::json::serialize(json_response);
            res.prepare_payload();
        } else {
            res.result(boost::beast::http::status::not_found);
            res.set(boost::beast::http::field::content_type, "text/plain");
            res.body() = "Not Found";
            res.prepare_payload();
        }
    } else {
        res.result(boost::beast::http::status::method_not_allowed);
        res.set(boost::beast::http::field::content_type, "text/plain");
        res.body() = "Method Not Allowed";
        res.prepare_payload();
    }

	std::string response_str{response_to_string(res)};

	std::cout << "Response: " << response_str << std::endl;

	return response_str;
}

http::request<http::string_body> HttpServer::string_to_request(std::string request)
{
	boost::beast::error_code ec;
	auto buf = boost::asio::buffer(std::move(request));
	http::request_parser<http::string_body> parser;
	parser.put(std::move(buf), ec);

	http::request<http::string_body> req{parser.release()};

	return req;
}

std::string HttpServer::response_to_string(const boost::beast::http::response<boost::beast::http::string_body>& res) const
{
    std::ostringstream ss;
    
    // Write status line
    ss << "HTTP/1.1 " << res.result_int() << " " 
       << res.reason() << "\r\n";
       
    // Write headers
    for(auto const& field : res) {
        ss << field.name_string() << ": " 
           << field.value() << "\r\n";
    }
    
    // Add empty line after headers
    ss << "\r\n";
    
    // Write body if present
    if(!res.body().empty()) 
	{
        ss << res.body().data() << "\r\n";
    }
    ss << "\r\n\r\n";
    
    return ss.str();
}
