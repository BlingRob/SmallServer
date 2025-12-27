#include "server.h"

#include <filesystem>
#include <format>

#include "base_tcp_server/base_tcp_server.hpp"
#include "default_configure.h"

using boost::asio::ip::tcp;

namespace fs = std::filesystem;

FTPServer::FTPServer(boost::asio::io_context& ioc, const toml::table& cfg, ILogger& logger)
    : BaseTCPServer(ioc,
                    {cfg["connections"]["host"].value_or(default_configures::kHost),
                     cfg["connections"]["ftp_port"].value_or(default_configures::kFtpPort)},
                    logger),
      root_directory_(cfg["work_dirs"]["ftp"].value_or(default_configures::kFtpWorkDir))
{
    logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);

    auto host{cfg["connections"]["host"].value_or(default_configures::kHost)};
    auto port{cfg["connections"]["ftp_port"].value_or(default_configures::kFtpPort)};

    fs::create_directories(root_directory_);

    logger_.Log(std::format("FTP Server {}:{} started", host, port), Severities::Info);
    logger_.Log(std::format("FTP Server directory {}", root_directory_), Severities::Info);
}

boost::asio::awaitable<void> FTPServer::request_handler(tcp_socket socket)
{
    logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);

    logger_.Log(std::format("FTP Connection from:  {}", socket.remote_endpoint().address().to_string()),
                Severities::Debug);

    FTPSession session(std::move(socket), root_directory_, logger_);

    co_return co_await session.Start();
}
