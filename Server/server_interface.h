/// \file server_interface.h
/// \brief Interface of all server in this project

#pragma once
#ifndef SERVER_INTERFACE_H
#define SERVER_INTERFACE_H

struct IServer
{
    virtual void Start() = 0;

    virtual void Stop() = 0;
};

#endif  // SERVER_INTERFACE_H