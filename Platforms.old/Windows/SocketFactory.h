#pragma once

#include "BasicSocketFactory.h"
#include "Socket.h"

class SocketFactory : public BasicSocketFactory
{
public:
    SocketFactory(Error& error);
    BasicSocket* CreateSocket(const IpAddress& ipAddress, Error& error) override;
};
