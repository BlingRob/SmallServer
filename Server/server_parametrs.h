/// \file server_parametrs.h
/// \brief Parametrs for start working server

#pragma once
#ifndef SERVER_PARAMETRS_H
#define SERVER_PARAMETRS_H

#include <string>

struct ServerParametrs
{
	ServerParametrs(const std::string& ip, const std::uint32_t port, const std::string& path): 
                  ip_(ip), 
				  port_(port), 
				  path_(path)
	{
	}

	std::string ip_;
	std::uint32_t port_;
	std::string path_;
};

#endif // SERVER_PARAMETRS_H