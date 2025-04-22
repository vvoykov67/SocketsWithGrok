#pragma once

#include "Error.h"
#include "ISocket.h"
#include <cstdint>
#include <memory>

// Forward declarations
class Socket;
class IpAddress;

class SocketListener {
public:
    SocketListener();
    ~SocketListener();

    // Create a TCP socket
    bool CreateSocket(Error& error);

    // Bind to an address and port
    bool Bind(const IpAddress& address, uint16_t port, Error& error);

    // Listen for incoming connections
    bool Listen(int backlog, Error& error);

    // Accept a client connection
    std::unique_ptr<Socket> Accept(Error& error);

    // Get the socket handle for select
    int GetHandle() const;

    // Close the socket
    void Close();

private:
    std::unique_ptr<ISocket> socket;
};
