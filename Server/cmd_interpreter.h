#pragma once

#include "server_configure.h"

#include <boost/program_options.hpp>

class CmdInterpreter
{
    public:

    std::unique_ptr<ServerOptions> CheckCMDParametrs(int getc, char** getv);

    private:

    std::unique_ptr<ServerOptions> fromConfigureFile(const boost::program_options::variables_map& map);

    std::unique_ptr<ServerOptions> fromArgs(const boost::program_options::variables_map& map);

    std::unique_ptr<ServerOptions> getDefault();
};