#include "parser.h"
#include "http_answers.h"

#include <iostream>
#include <fstream>

#include <boost/regex.hpp>
#include <boost/program_options.hpp>

std::unique_ptr<ServerOptions> Parser::CheckCMDParametrs(int getc, char** getv)
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

        return std::unique_ptr<ServerOptions>(new ServerOptions(vm["ip"].as<std::string>(), vm["port"].as<uint32_t>(), vm["directory"].as<std::string>()));
    }
    else
        return nullptr;

}

std::string Parser::Get(const std::string& request, const std::string& dir)
{
    boost::regex rgx("^(GET) ([a-zA-Z\\/\\.]+)", boost::regex_constants::ECMAScript);
    boost::smatch matchs = *boost::sregex_iterator(request.begin(), request.end(), rgx);
    std::string absPath = dir + matchs[2].str();
    std::cerr << absPath << std::endl;
    std::ifstream file(absPath.c_str());
    
    if (!file.is_open())
        return std::string(HTTP_ANSWERS::notFound);
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