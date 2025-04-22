#include "SocketFactory.h"

BasicSocket* SocketFactory::CreateSocket(const IpAddress& ipAddress, Error& error)
{
    try
    {
        BasicSocket* newSocket = new Socket(ipAddress);
        return newSocket;
    }
    catch (const std::exception& e)
    {
        error.SetError(ErrorCode::SocketFactoryCreateFailed, "Failed to create socket: " + std::string(e.what()));
        return nullptr;
    }
}
