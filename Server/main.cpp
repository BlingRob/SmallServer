
#include "http_server/server.h"
#include "server_interface.h"
#include "default_configure.h"
#include "logger_subsystem/boost_logger_impl.h"

#include <boost/asio/signal_set.hpp>

#include <format>

#include <iostream>

class Server : public IServer, public HttpServer
{
	public:

	Server(boost::asio::io_context& ioc, const ServerParameters& params, std::shared_ptr<ILogger> logger)
		  : logger_(std::move(logger))
		  , ioc_(ioc)
		  , sig_set_(ioc, SIGINT, SIGTERM)
		  /// THis start first
		  ,	HttpServer(ioc, params, *logger)
	{
		logger_->Log(std::format("Server {}:{} created", params.host_, params.port_), Severities::Info);

		sig_set_.async_wait([this](auto, auto){ ioc_.stop(); });
	}

	void Start() override
	{
		logger_->Log("Server started", Severities::Info);

		ioc_.run();
	}

	void Stop() override
	{
		logger_->Log("Server stopped", Severities::Info);

		ioc_.stop();
	}

	private:

	std::shared_ptr<ILogger> logger_;

	boost::asio::io_context& ioc_;

	boost::asio::signal_set sig_set_;

};

int main(int getc, char** getv)
{
	//InitLogger();

	// CmdInterpreter inter;
	
	try
	{
		boost::asio::io_context ioc;
		Server serv{ioc, {default_configures::IP, default_configures::Port}, std::make_shared<BoostLogger>()};
		serv.Start();
		// std::unique_ptr<BaseServerImpl> Serv;
		// ServerParameters ServerParameters{"127.0.0.1", 15000, "./"};
		// Serv = std::make_unique<BaseServerImpl>(ServerParameters);
		// std::jthread serverThread([&](){Serv->Start();});
		// std::cerr << "Finished" << std::endl;
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
