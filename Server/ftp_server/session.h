#pragma once

#include <boost/asio/awaitable.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <memory>
#include <string>

#include "logger_subsystem/logger_interface.h"

using tcp_socket = boost::asio::use_awaitable_t<>::as_default_on_t<boost::asio::ip::tcp::socket>;
using tcp_acceptor = boost::asio::use_awaitable_t<>::as_default_on_t<boost::asio::ip::tcp::acceptor>;

class FTPSession : public std::enable_shared_from_this<FTPSession>
{
   public:
    FTPSession(tcp_socket socket, const std::string& root_dir, ILogger& logger);

    boost::asio::awaitable<void> Start();

   private:
    boost::asio::awaitable<void> read_command();

    boost::asio::awaitable<void> process_command(const std::string& command_line);

    boost::asio::awaitable<void> send_response(int code, const std::string& message);

    boost::asio::awaitable<void> start_passive_mode();

    boost::asio::awaitable<void> accept_data_connection();

    boost::asio::awaitable<void> send_file_list();

    boost::asio::awaitable<void> change_directory(const std::string& dir);

    boost::asio::awaitable<void> send_file(const std::string& filename);

    boost::asio::awaitable<void> receive_file(const std::string& filename);

    boost::asio::awaitable<void> delete_file(const std::string& filename);

    boost::asio::awaitable<void> create_directory(const std::string& dirname);

    boost::asio::awaitable<void> remove_directory(const std::string& dirname);

    boost::asio::awaitable<void> get_file_size(const std::string& filename);

   private:
    ILogger& logger_;

    tcp_socket command_socket_;
    tcp_socket data_socket_;
    boost::asio::streambuf command_buffer_;

    std::string root_directory_;
    std::string current_directory_;
};
