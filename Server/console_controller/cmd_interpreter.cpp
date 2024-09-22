#include "cmd_interpreter.h"
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
        ("console,c", "Run server in console")
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

    ptree pt;
    read_json(pathConfigFile, pt);

    std::string ip = (pt.count("ip") == 0) ? default_configures::IP : pt.get<std::string>("ip");
    uint32_t port = (pt.count("port") == 0) ? default_configures::Port : pt.get<uint32_t>("port");
    std::string directory = (pt.count("directory") == 0) ? default_configures::Directory : pt.get<std::string>("directory");
    bool asConsole = (pt.count("console") == 0) ? default_configures::AsConsole : pt.get<bool>("console");

    return std::make_unique<ServerOptions>(ip, port, directory, asConsole);

}

std::unique_ptr<ServerOptions> CmdInterpreter::fromArgs(const boost::program_options::variables_map& map)
{
    std::string ip = (map.count("ip") == 0) ? default_configures::IP : map["ip"].as<std::string>();
    uint32_t port = (map.count("port") == 0) ? default_configures::Port : map["port"].as<uint32_t>();
    std::string directory = (map.count("directory") == 0) ? default_configures::Directory : map["directory"].as<std::string>();
    bool asConsole = (map.count("console") == 0) ? default_configures::AsConsole : map["console"].as<bool>();

    boost::regex rgx("^(\\d{1,3}).(\\d{1,3}).(\\d{1,3}).(\\d{1,3})$");
    boost::smatch res;

    if (!boost::regex_match(ip, res, rgx) || port > 65535)
        return getDefault();

    return std::make_unique<ServerOptions>(ip, port, directory, asConsole);
}

std::unique_ptr<ServerOptions> CmdInterpreter::getDefault()
{
    using namespace default_configures;
    return std::make_unique<ServerOptions>(IP, Port, Directory, AsConsole);
}