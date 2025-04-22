#pragma once

#include "BasicSocket.h"
#include "IpAddress.h"
#include <winsock2.h>
#include <ws2tcpip.h>

class Socket : public BasicSocket
{
public:
    Socket(const IpAddress& ipAddress);
    Socket(SOCKET sock);
    ~Socket();

    bool Connect(const IpAddress& address, Error& error) override;
    bool Bind(const IpAddress& address, Error& error) override;
    bool Listen(int backlog, Error& error) override;
    bool Accept(BasicSocket& socket, Error& error) override;
    bool Send(const Buffer& buffer, Error& error) override;
    bool Receive(Buffer& buffer, size_t maxLen, Error& error) override;
    void Close() override;

private:
    SOCKET SocketHandle;
    IpAddress IpAddress;
};
