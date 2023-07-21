#pragma once

#include <string>

struct ServerOptions
{
	ServerOptions(const std::string& ip, const std::uint32_t port, const std::string& path) : ip_(ip), port_(port), path_(path)
	{
	}

	std::string ip_;
	std::uint32_t port_;
	std::string path_;
};