#pragma once

#include <server_parameters.h>
#include <server_interface.h>
#include <base_tcp_server/base_tcp_server.hpp>

#include <boost/beast/http.hpp>
#include <boost/beast/core.hpp>

#include <string>

class HttpServer : public BaseTCPServer
{
	public:
		HttpServer(const ServerParameters& parameters, boost::asio::io_context& ioc);

	private:
		
		std::string user_handler(std::string request) override;

		boost::beast::http::request<boost::beast::http::string_body> string_to_request(std::string request);

		std::string response_to_string(const boost::beast::http::response<boost::beast::http::string_body>& res) const;
	
	private:

		ServerParameters server_parameters_;
};
