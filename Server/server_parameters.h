/// \file server_parametrs.h
/// \brief Parametrs for start working server

#pragma once
#ifndef SERVER_PARAMETRS_H
#define SERVER_PARAMETRS_H

#include <string>

struct ServerParameters
{
	ServerParameters(const std::string& ip, const std::uint16_t port, const std::uint32_t maxClients = 100): 
                  host_(ip), 
				  port_(port),
				  maxClients_(maxClients)
	{
	}

	std::string host_;
	std::uint16_t port_;
	std::uint32_t maxClients_;
};

#endif // SERVER_PARAMETRS_H