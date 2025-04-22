#pragma once

#include "RoleId.h"
#include "Buffer.h"
#include <cstdint>
#include <string>

#include <memory>

class Error; // Forward declaration for Error class
class Message; // Forward declaration for Message class

// Forward declarations
class IpAddress;
class ISocket;

class Client {
public:
    Client();
    ~Client();

    // Connect to the server
    bool Connect(const IpAddress& address, uint16_t port, Error& error);

    // Run the client loop until stopped
    void Run();

    // Stop the client
    void Stop();

    // Send HelloCommand to initialize session
    bool SendHelloCommand(RoleId roleId, Error& error);

    // Send GetFileList command (example)
    bool SendGetFileListCommand(const std::string& directory, Error& error);

    // Generic command sending
    bool SendCommand(uint16_t moduleId, uint16_t commandId, const Buffer& input, Error& error);

private:
    // Process incoming data
    void ProcessInput(Error& error);

    // Process outgoing data
    void ProcessOutput(Error& error);

    bool running;
    std::unique_ptr<ISocket> socket;
    Buffer inputBuffer;
    Buffer outputBuffer;
    bool hasOutputPending;
};
