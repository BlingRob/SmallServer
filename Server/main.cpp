
#include <boost/asio/signal_set.hpp>
#include <filesystem>
#include <format>
#include <iostream>
#include <toml.hpp>

#include "default_configure.h"
#include "ftp_server/server.h"
#include "ftp_server/web_gui_server.h"
#include "logger_subsystem/boost_logger_impl.h"
#include "server_interface.h"

class Server : public IServer
{
   public:
    Server(boost::asio::io_context& ioc, const toml::table& cfg, std::shared_ptr<ILogger> logger)
        : logger_(std::move(logger)),
          ioc_(ioc),
          sig_set_(ioc, SIGINT, SIGTERM),
          ftp_server_(ioc, cfg, *logger_),
          web_gui_ftp_server_(ioc, cfg, *logger_)
    {
        logger_->Log("Main server created", Severities::Info);

        sig_set_.async_wait([this](auto, auto) { ioc_.stop(); });
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

    FTPServer ftp_server_;

    WebGuiFtpServer web_gui_ftp_server_;
};

int main(int getc, char** getv)
{
    toml::table cfg;

    const std::string config_file_path{"cfg.toml"};

    if (!std::filesystem::exists(config_file_path))
    {
        throw std::runtime_error("Config file not found: " + config_file_path);
    }

    cfg = toml::parse_file(config_file_path);

    // CmdInterpreter inter;

    try
    {
        boost::asio::io_context ioc;
        Server serv{ioc, cfg, std::make_shared<BoostLogger>()};
        serv.Start();
        // std::unique_ptr<BaseServerImpl> Serv;
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
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
