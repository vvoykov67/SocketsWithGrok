#pragma once

#include "Error.h"
#include <cstdint>
#include <memory>

// Forward declarations
class IpAddress;
class Socket;
class Buffer;

class ISocket {
public:
    virtual ~ISocket() = default;

    // Create a TCP socket
    virtual bool CreateSocket(Error& error) = 0;

    // Bind to an address and port
    virtual bool Bind(const IpAddress& address, uint16_t port, Error& error) = 0;

    // Listen for incoming connections
    virtual bool Listen(int backlog, Error& error) = 0;

    // Accept a client connection
    virtual std::unique_ptr<Socket> Accept(Error& error) = 0;

    // Connect to a server
    virtual bool Connect(const IpAddress& address, uint16_t port, Error& error) = 0;

    // Receive data into a buffer
    virtual bool Receive(Buffer& buffer, Error& error) = 0;

    // Send data from a buffer
    virtual bool Send(Buffer& buffer, Error& error) = 0;

    // Set socket to non-blocking mode
    virtual bool SetNonBlocking(Error& error) = 0;

    // Enable address reuse
    virtual bool SetReuseAddress(Error& error) = 0;

    // Close the socket
    virtual void Close() = 0;

    // Get the socket handle for select
    virtual int GetHandle() const = 0;
};
