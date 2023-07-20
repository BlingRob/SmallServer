#pragma once

#include <boost/regex.hpp>
#include <boost/program_options.hpp>

namespace HTTP_ANSWERS
{
	const std::string not_found("HTTP/1.0 404 Not Found\r\nContent-length: 0\r\nContent-Type: text/html\r\n\r\n");
	const std::string ok("HTTP/1.0 200 OK\r\n");
};

struct server_options
{
	server_options(const std::string& ip, const std::uint32_t port, const std::string& path) :IP(ip), Port(port), Path(path)
	{
	}
	std::string IP;
	std::uint32_t Port;
	std::string Path;
};

class Parser
{
public:
	std::unique_ptr<server_options> CheckCMDParametrs(int getc, char** getv)
	{
		namespace po = boost::program_options;
		po::options_description desc("Options");
		desc.add_options()
			("ip,h", po::value<std::string>(), "Set IP address")
			("port,p", po::value<uint32_t>(), "Set port")
			("directory,d", po::value<std::string>(), "Set directory");

		po::variables_map vm;
		po::store(po::parse_command_line(getc, getv, desc), vm);

		if (vm.count("ip") && vm.count("port") && vm.count("directory"))
		{
			boost::regex rgx("^(\\d{1,3}).(\\d{1,3}).(\\d{1,3}).(\\d{1,3})$");
			boost::smatch res;
			if (!boost::regex_match(vm["ip"].as<std::string>(), res, rgx) || vm["port"].as<uint32_t>() > 65535)
				return nullptr;

			return std::unique_ptr<server_options>(new server_options(vm["ip"].as<std::string>(), vm["port"].as<uint32_t>(), vm["directory"].as<std::string>()));
		}
		else
			return nullptr;

	}
	std::string Get(const std::string& request, const std::string& dir)
	{
		boost::regex rgx("^(GET) ([a-zA-Z\\/\\.]+)", boost::regex_constants::ECMAScript);
		boost::smatch matchs = *boost::sregex_iterator(request.begin(), request.end(), rgx);
		std::string absPath = dir + matchs[2].str();
		std::cerr << absPath << std::endl;
		std::ifstream file(absPath.c_str());
		
		if (!file.is_open())
			return std::string(HTTP_ANSWERS::not_found);
		else 
		{
			std::string content;
			getline ( file, content, '\0' );
			std::string answer = HTTP_ANSWERS::ok;
			answer += "Content-length: " + std::to_string(content.size()) + "\r\n";
			answer += "Content-Type: text/html\r\n\r\n";
			answer += content;
			return answer;
		}
	}
};
