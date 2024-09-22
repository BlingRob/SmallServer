#pragma once
#include "server.h"

class ServerConsoleCommander
{
    public:

    ServerConsoleCommander(Server& serv);

    void CommandLoop();

    private:

    Server& serv_;
};