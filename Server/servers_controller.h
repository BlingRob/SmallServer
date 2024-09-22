/// \file servers_controller.h
/// \brief Controllers of servers group

#pragma once
#ifndef SERVERS_CONTROLLER_H
#define SERVERS_CONTROLLER_H

#include <memory>

class ServersController
{
    public:

    void StartAll();

    private:

    std::unique_ptr<HttpServer> httpServer_;
    //std::unique_ptr<HttpsServer> httpsServer_;
    //std::unique_ptr<WebsocketServer> websocketServer_;
    //std::unique_ptr<RestServer> restServer_;
    //std::unique_ptr<FtpServer> ftpServer_;
};

#endif // SERVERS_CONTROLLER_H