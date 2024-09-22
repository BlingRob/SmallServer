#pragma once

#include <boost/asio.hpp>

class ClientRequest
{
	public:

		ClientRequest(boost::asio::io_service& ioService, const std::string& path);

		boost::asio::ip::tcp::socket& Socket();

		void Start();

		~ClientRequest();

	private:

		void handleRead(const boost::system::error_code& error,
						size_t bytes_transferred);

		void handleWrite(const boost::system::error_code& error);

	private:

		boost::asio::ip::tcp::socket clientSock_;

		boost::asio::streambuf buf_;
		std::string rootPath_;
};