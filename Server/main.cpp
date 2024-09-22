// #include "cmd_interpreter.h"
// #include "server.h"
// #include "server_console_commander.h"
// #include "logger.h"

#if !defined(WIN32) || !defined(_WIN32) || !defined(__WIN32) && !defined(__CYGWIN__)
//#include <unistd.h>
#endif

// #include <signal.h>

#include "base_server_impl.h"

#include <iostream>
#include <thread>
#include <memory>

// std::shared_ptr<Server> Serv;

// void SigHandler(int signum)
// {
//   	if(signum == SIGKILL)
//   	{
// 		Serv->Stop();
// 		std::cin.putback('q');
// 		std::cout << "Killed" << std::endl;
// 	}
// }


int main(int getc, char** getv)
{
	//InitLogger();

	// CmdInterpreter inter;
	
	try
	{
		std::unique_ptr<BaseServerImpl> Serv;
		ServerParametrs serverParametrs{"127.0.0.1", 15000, "./"};
		Serv = std::make_unique<BaseServerImpl>(serverParametrs);
		std::jthread serverThread([&](){Serv->Start();});
		std::cerr << "Finished" << std::endl;
	}
		// std::unique_ptr<ServerOptions> srvOpt = inter.CheckCMDParametrs(getc, getv);
		// bool asConsole = srvOpt->asConsoleApp_;
		// Serv = std::make_shared<Server>(std::move(srvOpt));

	// #if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	// 	asConsole = true;
	// #endif

	// 	if(asConsole)
	// 	{
	// 		//BOOST_LOG_TRIVIAL(info) << "Start work server";
			
	// 		//ServerConsoleCommander commander(*Serv);
	// 		//commander.CommandLoop();
	// 		if(serverThread.joinable())
	// 		{
	// 			serverThread.join();
	// 		}
	// 	}
	// 	#if !defined(WIN32) || !defined(_WIN32) || !defined(__WIN32) && !defined(__CYGWIN__)
	// 	else 
	// 	{
	// 		//signal(SIGKILL, SigHandler);
	// 		//daemon(0, 0);
	// 		Serv->Run();
	// 	}
	// 	#endif
	// }
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
