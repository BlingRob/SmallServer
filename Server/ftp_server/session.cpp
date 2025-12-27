#pragma once

#include "session.h"

#include <boost/asio/read_until.hpp>
#include <boost/asio/write.hpp>
#include <filesystem>
#include <format>
#include <fstream>
#include <memory>
#include <string>

namespace fs = std::filesystem;

FTPSession::FTPSession(tcp_socket socket, const std::string& root_dir, ILogger& logger)
    : logger_(logger),
      command_socket_(std::move(socket)),
      data_socket_(command_socket_.get_executor()),
      root_directory_(root_dir)
{
    logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);

    current_directory_ = "/";
}

boost::asio::awaitable<void> FTPSession::Start()
{
    logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);

    co_await send_response(220, "Welcome to FTP Server with Web GUI");
    co_await read_command();
}

boost::asio::awaitable<void> FTPSession::read_command()
{
    logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);

    while (true)
    {
        std::size_t n{co_await boost::asio::async_read_until(
            command_socket_, command_buffer_, "\r\n", boost::asio::use_awaitable)};
        if (!n)
        {
            break;
        }

        std::istream is(&command_buffer_);
        std::string command_line;
        std::getline(is, command_line);

        if (!command_line.empty() && command_line.back() == '\r')
        {
            command_line.pop_back();
        }

        co_await process_command(command_line);
    }
}

boost::asio::awaitable<void> FTPSession::process_command(const std::string& command_line)
{
    logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);

    logger_.Log(std::format("FTP Command: {}", command_line), Severities::Debug);

    std::istringstream iss(command_line);
    std::string command;
    iss >> command;

    std::transform(command.begin(), command.end(), command.begin(), ::toupper);

    if (command == "USER")
    {
        co_await send_response(331, "User name okay, need password");
    }
    else if (command == "PASS")
    {
        co_await send_response(230, "User logged in successfully");
    }
    else if (command == "SYST")
    {
        co_await send_response(215, "UNIX Type: L8");
    }
    else if (command == "FEAT")
    {
        co_await send_response(211, "No features");
    }
    else if (command == "PWD")
    {
        co_await send_response(257, "\"" + current_directory_ + "\"");
    }
    else if (command == "TYPE")
    {
        std::string type;
        iss >> type;
        co_await send_response(200, "Type set to " + type);
    }
    else if (command == "PASV")
    {
        start_passive_mode();
    }
    else if (command == "LIST")
    {
        send_file_list();
    }
    else if (command == "CWD")
    {
        std::string dir;
        iss >> dir;
        change_directory(dir);
    }
    else if (command == "QUIT")
    {
        co_await send_response(221, "Goodbye");
        command_socket_.close();
    }
    else if (command == "RETR")
    {
        std::string filename;
        iss >> filename;
        send_file(filename);
    }
    else if (command == "STOR")
    {
        std::string filename;
        iss >> filename;
        receive_file(filename);
    }
    else if (command == "DELE")
    {
        std::string filename;
        iss >> filename;
        delete_file(filename);
    }
    else if (command == "MKD")
    {
        std::string dirname;
        iss >> dirname;
        create_directory(dirname);
    }
    else if (command == "RMD")
    {
        std::string dirname;
        iss >> dirname;
        remove_directory(dirname);
    }
    else if (command == "SIZE")
    {
        std::string filename;
        iss >> filename;
        get_file_size(filename);
    }
    else
    {
        co_await send_response(502, "Command not implemented");
    }
}

boost::asio::awaitable<void> FTPSession::send_response(int code, const std::string& message)
{
    logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);

    std::string response{std::format("{} {}\r\n", code, message)};

    co_await boost::asio::async_write(command_socket_, boost::asio::buffer(response), boost::asio::use_awaitable);
}

boost::asio::awaitable<void> FTPSession::start_passive_mode()
{
    logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);

    try
    {
        auto executor{co_await boost::asio::this_coro::executor};

        tcp_acceptor passive_acceptor(executor, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 0));

        unsigned short port{passive_acceptor.local_endpoint().port()};
        auto local_endpoint{command_socket_.local_endpoint()};
        std::string ip{local_endpoint.address().to_string()};

        std::replace(ip.begin(), ip.end(), '.', ',');

        auto p1{static_cast<unsigned short>(port / 256)};
        auto p2{static_cast<unsigned short>(port % 256)};

        std::string response{std::format("227 Entering Passive Mode ({},{},{})", ip, p1, p2)};
        co_await send_response(227, response);

        data_socket_ = co_await passive_acceptor.async_accept();

        co_return;
    }
    catch (const std::exception& e)
    {
        logger_.Log(std::format("Passive mode error: {}", e.what()), Severities::Error);
    }

    co_return co_await send_response(425, "Can't open data connection");
}

boost::asio::awaitable<void> FTPSession::send_file_list()
{
    logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);

    try
    {
        fs::path dir_path{root_directory_ + current_directory_};

        if (!fs::exists(dir_path) || !fs::is_directory(dir_path))
        {
            co_return co_await send_response(550, "Directory not found");
        }

        co_await send_response(150, "Here comes the directory listing");

        std::string listing;
        for (const auto& entry : fs::directory_iterator(dir_path))
        {
            auto filename{entry.path().filename().string()};

            if (fs::is_directory(entry.status()))
            {
                listing += "drwxr-xr-x 1 owner group " + std::to_string(fs::file_size(entry)) + " Jan 1 00:00 " +
                           filename + "\r\n";
            }
            else
            {
                listing += "-rw-r--r-- 1 owner group " + std::to_string(fs::file_size(entry)) + " Jan 1 00:00 " +
                           filename + "\r\n";
            }
        }

        if (listing.empty())
        {
            listing = "No files in directory\r\n";
        }

        co_await boost::asio::async_write(data_socket_, boost::asio::buffer(listing), boost::asio::use_awaitable);
        data_socket_.close();

        co_return co_await send_response(226, "Directory send OK");
    }
    catch (const std::exception& e)
    {
        logger_.Log(std::format("LIST error: {}", e.what()), Severities::Error);
    }

    co_return co_await send_response(550, "Failed to list directory");
}

boost::asio::awaitable<void> FTPSession::change_directory(const std::string& dir)
{
    logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);

    try
    {
        fs::path new_path{fs::path(current_directory_) / dir};
        fs::path full_path{root_directory_ / new_path};

        if (fs::exists(full_path) && fs::is_directory(full_path))
        {
            current_directory_ = new_path.string();
            if (current_directory_.back() != '/')
            {
                current_directory_ += "/";
            }
            co_return co_await send_response(250, "Directory changed to " + current_directory_);
        }
        else
        {
            co_return co_await send_response(550, "Directory not found");
        }
    }
    catch (const std::exception& e)
    {
        logger_.Log(std::format("CWD error: {}", e.what()), Severities::Error);
    }

    co_return co_await send_response(550, "Failed to change directory");
}

boost::asio::awaitable<void> FTPSession::send_file(const std::string& filename)
{
    logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);

    try
    {
        fs::path file_path{root_directory_ + current_directory_ + filename};

        if (!fs::exists(file_path))
        {
            co_return co_await send_response(550, "File not found");
        }

        co_await send_response(150, "Opening BINARY mode data connection");

        std::ifstream file(file_path.string(), std::ios::binary);
        if (!file)
        {
            throw std::runtime_error("Cannot open file");
        }

        std::vector<char> buffer(4096);
        while (file)
        {
            file.read(buffer.data(), buffer.size());
            std::streamsize bytes_read = file.gcount();

            if (bytes_read > 0)
            {
                boost::asio::write(data_socket_, boost::asio::buffer(buffer.data(), bytes_read));
            }
        }

        data_socket_.close();
        co_return co_await send_response(226, "Transfer complete");
    }
    catch (const std::exception& e)
    {
        logger_.Log(std::format("RETR error: {}", e.what()), Severities::Error);
    }

    co_return co_await send_response(550, "Failed to send file");
}

boost::asio::awaitable<void> FTPSession::receive_file(const std::string& filename)
{
    logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);

    try
    {
        fs::path file_path = root_directory_ + current_directory_ + filename;

        co_await send_response(150, "Ready to receive file");

        std::ofstream file(file_path.string(), std::ios::binary);
        if (!file)
        {
            throw std::runtime_error("Cannot create file");
        }

        std::vector<char> buffer(4096);
        boost::system::error_code ec;

        while (!ec)
        {
            std::size_t bytes_read = data_socket_.read_some(boost::asio::buffer(buffer), ec);

            if (bytes_read > 0)
            {
                file.write(buffer.data(), bytes_read);
            }
        }

        if (ec != boost::asio::error::eof)
        {
            throw boost::system::system_error(ec);
        }

        data_socket_.close();
        co_return co_await send_response(226, "Transfer complete");
    }
    catch (const std::exception& e)
    {
        logger_.Log(std::format("STOR error: {}", e.what()), Severities::Error);
    }

    co_return co_await send_response(550, "Failed to receive file");
}

boost::asio::awaitable<void> FTPSession::delete_file(const std::string& filename)
{
    logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);

    try
    {
        fs::path file_path = root_directory_ + current_directory_ + filename;

        if (fs::remove(file_path))
        {
            co_return co_await send_response(250, "File deleted successfully");
        }
        else
        {
            co_return co_await send_response(550, "File not found or cannot be deleted");
        }
    }
    catch (const std::exception& e)
    {
        logger_.Log(std::format("DELE error: {}", e.what()), Severities::Error);
    }

    co_return co_await send_response(550, "Failed to delete file");
}

boost::asio::awaitable<void> FTPSession::create_directory(const std::string& dirname)
{
    logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);

    try
    {
        fs::path dir_path = root_directory_ + current_directory_ + dirname;

        if (fs::create_directory(dir_path))
        {
            co_return co_await send_response(257, "Directory created successfully");
        }
        else
        {
            co_return co_await send_response(550, "Failed to create directory");
        }
    }
    catch (const std::exception& e)
    {
        logger_.Log(std::format("MKD error: {}", e.what()), Severities::Error);
    }

    co_return co_await send_response(550, "Failed to create directory");
}

boost::asio::awaitable<void> FTPSession::remove_directory(const std::string& dirname)
{
    logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);

    try
    {
        fs::path dir_path = root_directory_ + current_directory_ + dirname;

        if (fs::remove_all(dir_path) > 0)
        {
            co_return co_await send_response(250, "Directory removed successfully");
        }
        else
        {
            co_return co_await send_response(550, "Directory not found or cannot be removed");
        }
    }
    catch (const std::exception& e)
    {
        logger_.Log(std::format("RMD error: {}", e.what()), Severities::Error);
    }

    co_return co_await send_response(550, "Failed to remove directory");
}

boost::asio::awaitable<void> FTPSession::get_file_size(const std::string& filename)
{
    logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);

    try
    {
        fs::path file_path{root_directory_ + current_directory_ + filename};

        if (!fs::exists(file_path))
        {
            co_return co_await send_response(550, "File not found");
        }

        auto size{fs::file_size(file_path)};
        co_return co_await send_response(213, std::to_string(size));
    }
    catch (const std::exception& e)
    {
        logger_.Log(std::format("SIZE error: {}", e.what()), Severities::Error);
    }
    co_return co_await send_response(550, "Failed to get file size");
}
