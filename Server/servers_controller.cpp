#include "servers_controller.h"


void ServersController::StartAll()
{
    httpServer_->Start();
    //httpsServer_->Start();
    //websocketServer_->Start();
    //restServer_->Start();
    //ftpServer_->Start();
}