#include "server.h"

#include <iostream>
#include <sstream>

namespace http = boost::beast::http;

HttpServer::HttpServer(const ServerParameters& parameters, boost::asio::io_context& ioc)
					  : server_parameters_{parameters}
					  , BaseTCPServer(parameters, ioc)
{

}

std::string HttpServer::user_handler(std::string request)
{
    // http::request<http::string_view_body> req{http::verb::post, "/", 11};
    // req.set(http::field::host, "example.com");
    
    // // Convert string to string_view first
    // auto view = boost::string_view(body_str);
    // req.body().data(view);
    // req.prepare_payload();
    // return req;

	std::cout << "Request: " << request << std::endl;

	http::request<http::string_body> req{string_to_request(std::move(request))};

	// Prepare the response
	http::response<http::string_body> res;
	res.result(http::status::ok);
	res.set(http::field::content_length, "Hello");
	res.prepare_payload();

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
    ss << res.result_int() << " " 
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
        ss << res.body().data();
    }
    
    return ss.str();
}
