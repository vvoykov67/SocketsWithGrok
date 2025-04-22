#pragma once

#include "BasicSocket.h"
#include "IpAddress.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class Socket : public BasicSocket
{
public:
    Socket(const IpAddress& ipAddress);
    Socket(int sock);
    ~Socket();

    bool Connect(const IpAddress& address, Error& error) override;
    bool Bind(const IpAddress& address, Error& error) override;
    bool Listen(int backlog, Error& error) override;
    bool Accept(BasicSocket& socket, Error& error) override;
    bool Send(const Buffer& buffer, Error& error) override;
    bool Receive(Buffer& buffer, size_t maxLen, Error& error) override;
    void Close() override;

private:
    int SocketHandle;
    IpAddress IpAddress;
};
