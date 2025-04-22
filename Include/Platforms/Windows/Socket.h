#pragma once

#include "ISocket.h"
#include <winsock2.h>
#include <ws2tcpip.h>

class Socket : public ISocket {
public:
    Socket();
    ~Socket() override;

    bool CreateSocket(Error& error) override;
    bool Bind(const IpAddress& address, uint16_t port, Error& error) override;
    bool Listen(int backlog, Error& error) override;
    std::unique_ptr<Socket> Accept(Error& error) override;
    bool Connect(const IpAddress& address, uint16_t port, Error& error) override;
    bool Receive(Buffer& buffer, Error& error) override;
    bool Send(Buffer& buffer, Error& error) override;
    bool SetNonBlocking(Error& error) override;
    bool SetReuseAddress(Error& error) override;
    void Close() override;
    int GetHandle() const override;

private:
    SOCKET socketHandle;
    bool isInitialized;
};
