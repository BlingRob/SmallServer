#include "cmd_interpreter.h"
#include "server.h"
#include "server_console_commander.h"

#include <unistd.h>
#include <signal.h>

#include <iostream>
#include <thread>
#include <memory>

std::shared_ptr<Server> Serv;

void SigHandler(int signum)
{
  	if(signum == SIGKILL)
  	{
		Serv->Stop();
		std::cin.putback('q');
		std::cout << "Killed" << std::endl;
	}
}


int main(int getc, char** getv)
{
	CmdInterpreter inter;
	
	try
	{
		std::unique_ptr<ServerOptions> srvOpt = inter.CheckCMDParametrs(getc, getv);
		bool asConsole = srvOpt->asConsoleApp_;
		Serv = std::make_shared<Server>(std::move(srvOpt));
		if(asConsole)
		{
			std::jthread serverThread([&](){Serv->Run();});
			ServerConsoleCommander commander(*Serv);
			commander.CommandLoop();
		}
		else 
		{
			signal(SIGKILL, SigHandler);
			daemon(0, 0);
			Serv->Run();
		}
		
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
