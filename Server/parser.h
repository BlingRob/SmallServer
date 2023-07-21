#pragma once

#include <memory>
#include <string>

struct ServerOptions
{
	ServerOptions(const std::string& ip, const std::uint32_t port, const std::string& path) : IP(ip), Port(port), Path(path)
	{
	}

	std::string IP;
	std::uint32_t Port;
	std::string Path;
};

class Parser
{

public:

	std::unique_ptr<ServerOptions> CheckCMDParametrs(int getc, char** getv);

	std::string Get(const std::string& request, const std::string& dir);
};
