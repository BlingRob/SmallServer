#include "cmd_interpreter.h"
#include "server.h"
#include "server_console_commander.h"

#include <unistd.h>
#include <iostream>
#include <thread>

int main(int getc, char** getv)
{
	CmdInterpreter inter;
	
	try
	{
		std::unique_ptr<ServerOptions> srvOpt = inter.CheckCMDParametrs(getc, getv);
		bool asConsole = srvOpt->asConsoleApp_;
		Server serv(std::move(srvOpt));
		if(asConsole)
		{
			std::jthread serverThread([&](){serv.Run();});
			ServerConsoleCommander commander(serv);
			commander.CommandLoop();
		}
		else 
		{
			daemon(0, 0);
			serv.Run();
		}
		
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
