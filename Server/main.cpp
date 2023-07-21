#include "parser.h"
#include "server.h"

#include <unistd.h>
#include <iostream>

int main(int getc,char** getv)
{
	//getopt thread
        daemon(0, 0);

	Parser prs;
	
	try
	{
		std::unique_ptr<ServerOptions> SrvOpt = prs.CheckCMDParametrs(getc, getv);
		Server serv(std::move(SrvOpt));
		serv.Run();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
