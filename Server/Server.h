#pragma once

#include "SocketListener.h"
#include "Buffer.h"
#include "Error.h"
#include <cstdint>
#include <memory>
#include <vector>

// Forward declarations
class IpAddress;
class Socket;

class Server {
public:
    Server();
    ~Server();

    // Initialize the server by binding to an address and port
    bool Initialize(const IpAddress& address, uint16_t port, Error& error);

    // Run the server loop
    void Run();

    // Stop the server
    void Stop();

private:
    struct ClientData {
        std::unique_ptr<Socket> socket;
        Buffer inputBuffer;
        Buffer outputBuffer;
        bool pendingOutput;
        uint32_t roleId;

        ClientData(std::unique_ptr<Socket> s);
    };

    // Receive data from a client
    void Receive(ClientData& client, Error& error);

    // Send data to a client
    void Send(ClientData& client, Error& error);

    bool running;
    SocketListener listener;
    std::vector<ClientData> clients;
};
