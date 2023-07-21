#include "cmd_interpreter.h"
#include "utils.h"
#include "default_configure.h"

#include <filesystem>

#include <boost/regex.hpp>
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"

std::unique_ptr<ServerOptions> CmdInterpreter::CheckCMDParametrs(int getc, char** getv)
{
    namespace po = boost::program_options;
    po::options_description desc("Options");
    desc.add_options()
        ("configure,f", po::value<std::string>(), "Configure file with server configure parametrs")
        ("ip,h", po::value<std::string>(), "Set IP address")
        ("port,p", po::value<uint32_t>(), "Set port")
        ("directory,d", po::value<std::string>(), "Set directory");

    po::variables_map vm;
    po::store(po::parse_command_line(getc, getv, desc), vm);

    if(vm.count("configure"))
    {
        return fromConfigureFile(vm);
    }
    if (vm.count("ip") || vm.count("port") || vm.count("directory"))
    {
        return fromArgs(vm);
    }
    else
    {
        return getDefault();
    }

}

std::unique_ptr<ServerOptions> CmdInterpreter::fromConfigureFile(const boost::program_options::variables_map& map)
{
    using boost::property_tree::ptree;

    std::string pathConfigFile = map["configure"].as<std::string>();

    //std::string configJson = LoadTextFile(pathConfigFile);

    ptree pt;
    read_json(pathConfigFile, pt);

    std::string ip = (pt.count("ip") == 0) ? default_configures::IP : pt.get<std::string>("ip");
    uint32_t port = (pt.count("port") == 0) ? default_configures::Port : pt.get<uint32_t>("port");
    std::string directory = (pt.count("directory") == 0) ? default_configures::Directory : pt.get<std::string>("directory");

    return std::unique_ptr<ServerOptions>(new ServerOptions(ip, port, directory));

}

std::unique_ptr<ServerOptions> CmdInterpreter::fromArgs(const boost::program_options::variables_map& map)
{
    std::string ip = (map.count("ip") == 0) ? default_configures::IP : map["ip"].as<std::string>();
    uint32_t port = (map.count("port") == 0) ? default_configures::Port : map["port"].as<uint32_t>();
    std::string directory = (map.count("directory") == 0) ? default_configures::Directory : map["directory"].as<std::string>();

    boost::regex rgx("^(\\d{1,3}).(\\d{1,3}).(\\d{1,3}).(\\d{1,3})$");
    boost::smatch res;

    if (!boost::regex_match(ip, res, rgx) || port > 65535)
        return getDefault();

    return std::unique_ptr<ServerOptions>(new ServerOptions(ip, port, directory));
}

std::unique_ptr<ServerOptions> CmdInterpreter::getDefault()
{
    return std::unique_ptr<ServerOptions>(new ServerOptions(default_configures::IP, default_configures::Port, default_configures::Directory));
}