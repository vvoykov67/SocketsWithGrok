#pragma once

#include "BasicSocketFactory.h"
#include "Socket.h"

class Error; // Forward declaration

class SocketFactory : public BasicSocketFactory
{
public:
    BasicSocket* CreateSocket(const IpAddress& ipAddress, Error& error) override;
};
