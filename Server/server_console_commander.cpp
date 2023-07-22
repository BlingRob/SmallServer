#include "server_console_commander.h"

#include <string>
#include <iostream>

ServerConsoleCommander::ServerConsoleCommander(Server& serv) : 
                        serv_(serv) {}

void ServerConsoleCommander::CommandLoop()
{
    std::string command{"working"};
    do
    {
        std::cin >> command;
    }
    while(command[0] != 'q');

    serv_.Stop();
}