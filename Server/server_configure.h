#pragma once

#include <string>

struct ServerOptions
{
	ServerOptions(const std::string& ip, const std::uint32_t port, const std::string& path, bool asConsole) : 
                  ip_(ip), port_(port), path_(path), asConsoleApp_(asConsole)
	{
	}

	std::string ip_;
	std::uint32_t port_;
	std::string path_;
    bool asConsoleApp_;
};