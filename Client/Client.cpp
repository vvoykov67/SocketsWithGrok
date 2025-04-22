#include "Client.h"
#include "IpAddress.h"
#include "Buffer.h"
#include "Socket.h" // Platform-specific Socket, included via build system
#include <iostream>
#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/select.h>
#include <unistd.h>
#endif

Client::Client() 
    : running(false), socket(std::make_unique<Socket>()), inputBuffer(4096), outputBuffer(4096), hasOutputPending(false) {}

Client::~Client() {
    Stop();
}

bool Client::Connect(const IpAddress& address, uint16_t port, Error& error) {
    if (!socket->CreateSocket(error)) {
        return false;
    }
    if (!socket->SetNonBlocking(error)) {
        socket->Close();
        return false;
    }
    if (!socket->Connect(address, port, error)) {
        error.SetError(ErrorCode::SocketConnectFailed, "Failed to connect to server");
        socket->Close();
        return false;
    }
    error.SetError(ErrorCode::Success, "");
    return true;
}

void Client::Run() {
    running = true;
    fd_set readFds, writeFds;

    while (running) {
        FD_ZERO(&readFds);
        FD_ZERO(&writeFds);

        int socketFd = socket->GetHandle();
        FD_SET(socketFd, &readFds);
        if (hasOutputPending) {
            FD_SET(socketFd, &writeFds);
        }

        // Select with 1-second timeout to allow stopping
        struct timeval timeout = {1, 0};
        int result = select(socketFd + 1, &readFds, &writeFds, nullptr, &timeout);
        if (result < 0) {
            std::cerr << "Select failed: " << 
#ifdef _WIN32
                WSAGetLastError() 
#else
                errno
#endif
                << std::endl;
            continue;
        }

        Error error;
        if (FD_ISSET(socketFd, &readFds)) {
            ProcessInput(error);
            if (error.GetCode() != ErrorCode::Success) {
                std::cerr << "Receive error: " << error.GetText() << std::endl;
                running = false;
            }
        }

        if (running && FD_ISSET(socketFd, &writeFds) && hasOutputPending) {
            ProcessOutput(error);
            if (error.GetCode() != ErrorCode::Success) {
                std::cerr << "Send error: " << error.GetText() << std::endl;
                running = false;
            }
        }
    }
}

void Client::Stop() {
    running = false;
    if (socket) {
        socket->Close();
        socket.reset();
    }
    inputBuffer.Clear();
    outputBuffer.Clear();
    hasOutputPending = false;
}

bool Client::SendHelloCommand(RoleId roleId, Error& error) {
    Buffer input(4096);
    if (!input.Write(static_cast<uint32_t>(roleId), Message::Endian::Native, error)) {
        error.SetError(ErrorCode::BufferWriteRawFailed, "Failed to write RoleId");
        return false;
    }
    return SendCommand(1, 1, input, error); // ModuleId=1, CommandId=1
}

bool Client::SendGetFileListCommand(const std::string& directory, Error& error) {
    Buffer input(4096);
    // Serialize directory path (example)
    uint32_t length = static_cast<uint32_t>(directory.size());
    if (!input.Write(length, Message::Endian::Native, error) ||
        !input.Write(reinterpret_cast<const uint8_t*>(directory.c_str()), length, error)) {
        error.SetError(ErrorCode::BufferWriteRawFailed, "Failed to write directory path");
        return false;
    }
    return SendCommand(3, 1, input, error); // ModuleId=3 (FileModule), CommandId=1
}

bool Client::SendCommand(uint16_t moduleId, uint16_t commandId, const Buffer& input, Error& error) {
    // Create and serialize message
    Message message;
    message.SetModuleId(moduleId);
    message.SetCommandId(commandId);
    message.SetSize(12 + input.GetSize()); // Header + body
    message.SetCrc(0); // Replace with actual CRC if implemented

    Buffer messageBuffer(4096);
    if (!message.Serialize(messageBuffer, error)) {
        error.SetError(ErrorCode::ClientSerializeFailed, "Failed to serialize command");
        return false;
    }

    // Queue for sending
    outputBuffer = std::move(messageBuffer);
    hasOutputPending = true;
    error.SetError(ErrorCode::Success, "");
    return true;
}

void Client::ProcessInput(Error& error) {
    // Receive data
    if (!socket->Receive(inputBuffer, error)) {
        return;
    }

    // Deserialize message
    Message message;
    if (!message.Deserialize(inputBuffer, error)) {
        error.SetError(ErrorCode::ClientReceiveFailed, "Failed to deserialize response");
        return;
    }

    // Process response (example: print for demonstration)
    if (message.GetModuleId() == 1 && message.GetCommandId() == 1) {
        // HelloCommand response
        Buffer responseBuffer(4096);
        const std::vector<uint8_t>& body = message.GetBody();
        responseBuffer.Write(body.data(), body.size(), error);
        if (error.GetCode() == ErrorCode::Success) {
            uint32_t roleIdRaw;
            if (responseBuffer.Read(roleIdRaw, message.GetEndian(), error)) {
                std::cout << "Received HelloCommand response, RoleId: " << roleIdRaw << std::endl;
            }
        }
    } else if (message.GetModuleId() == 3 && message.GetCommandId() == 1) {
        // GetFileList response (example: assume list of strings)
        Buffer responseBuffer(4096);
        const std::vector<uint8_t>& body = message.GetBody();
        responseBuffer.Write(body.data(), body.size(), error);
        if (error.GetCode() == ErrorCode::Success) {
            uint32_t count;
            if (responseBuffer.Read(count, message.GetEndian(), error)) {
                std::cout << "Received GetFileList response, file count: " << count << std::endl;
                for (uint32_t i = 0; i < count && error.GetCode() == ErrorCode::Success; ++i) {
                    uint32_t length;
                    if (responseBuffer.Read(length, message.GetEndian(), error)) {
                        std::vector<uint8_t> name(length);
                        if (responseBuffer.Read(name.data(), length, error)) {
                            std::string fileName(name.begin(), name.end());
                            std::cout << "File " << i + 1 << ": " << fileName << std::endl;
                        }
                    }
                }
            }
        }
    }

    if (error.GetCode() != ErrorCode::Success) {
        std::cerr << "Failed to process response: " << error.GetText() << std::endl;
    } else {
        error.SetError(ErrorCode::Success, "");
    }
    inputBuffer.Clear();
}

void Client::ProcessOutput(Error& error) {
    if (!socket->Send(outputBuffer, error)) {
        return;
    }
    outputBuffer.Clear();
    hasOutputPending = false;
    error.SetError(ErrorCode::Success, "");
}
