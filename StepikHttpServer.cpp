#include "Headers.h"
#include "Parser.h"
#include "Server.h"
#include <unistd.h>


int main(int getc,char** getv)
{
	//getopt thread
        daemon(0, 0);
	Parser prs;
	try
	{
		std::unique_ptr<server_options> SrvOpt = prs.CheckCMDParametrs(getc, getv);
		Server serv(std::move(SrvOpt));
		serv.run();
	}
	catch(std::exception e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
