#include "Server.h"
#include "Socket.h"
#include "IpAddress.h"
#include "Message.h"
#include <iostream>
#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/select.h>
#include <unistd.h>
#endif

Server::ClientData::ClientData(std::unique_ptr<Socket> s)
    : socket(std::move(s)), inputBuffer(4096), outputBuffer(4096), pendingOutput(false), roleId(0) {}

Server::Server() : running(false), listener() {}

Server::~Server() {
    Stop();
}

bool Server::Initialize(const IpAddress& address, uint16_t port, Error& error) {
    if (!listener.CreateSocket(error)) {
        error.AppendContext("Failed to create socket for listener");
        return false;
    }
    if (error.IsSet()) return false;

    if (!listener.Bind(address, port, error)) {
        error.AppendContext("Failed to bind to address: " + address.GetAddress() + ":" + std::to_string(port));
        return false;
    }
    if (error.IsSet()) return false;

    if (!listener.Listen(10, error)) {
        error.AppendContext("Failed to listen on port: " + std::to_string(port));
        return false;
    }
    if (error.IsSet()) return false;

    error.SetError(ErrorCode::Success, "");
    return true;
}

void Server::Run() {
    running = true;
    fd_set readFds, writeFds;

    while (running) {
        FD_ZERO(&readFds);
        FD_ZERO(&writeFds);

        // Add listener socket to read set
        int listenerFd = listener.GetHandle();
        FD_SET(listenerFd, &readFds);
        int maxFd = listenerFd;

        // Add client sockets to read and write sets
        for (const auto& client : clients) {
            int clientFd = client.socket->GetHandle();
            FD_SET(clientFd, &readFds);
            if (client.pendingOutput) {
                FD_SET(clientFd, &writeFds);
            }
            maxFd = (std::max)(maxFd, clientFd);
        }

        // Select with 1-second timeout to allow stopping
        struct timeval timeout = {1, 0};
        int result = select(maxFd + 1, &readFds, &writeFds, nullptr, &timeout);
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
        // Check for new connections
        if (FD_ISSET(listenerFd, &readFds)) {
            auto clientSocket = listener.Accept(error);
            if (error.IsSet()) {
                std::cerr << "Accept error: " << error.GetText() << std::endl;
                continue;
            }
            if (clientSocket) {
                clients.emplace_back(std::move(clientSocket));
                std::cout << "New client connected, total clients: " << clients.size() << std::endl;
            }
        }

        // Process existing clients
        for (auto it = clients.begin(); it != clients.end();) {
            int clientFd = it->socket->GetHandle();
            if (FD_ISSET(clientFd, &readFds)) {
                Receive(*it, error);
                if (error.IsSet()) {
                    std::cerr << "Receive error for client: " << error.GetText() << std::endl;
                    it = clients.erase(it);
                    std::cout << "Client disconnected, remaining clients: " << clients.size() << std::endl;
                    continue;
                }
            }

            if (running && FD_ISSET(clientFd, &writeFds) && it->pendingOutput) {
                Send(*it, error);
                if (error.IsSet()) {
                    std::cerr << "Send error for client: " << error.GetText() << std::endl;
                    it = clients.erase(it);
                    std::cout << "Client disconnected, remaining clients: " << clients.size() << std::endl;
                    continue;
                }
            }

            ++it;
        }
    }
}

void Server::Stop() {
    running = false;
    clients.clear();
    listener.Close();
}

void Server::Receive(ClientData& client, Error& error) {
    if (!client.socket->Receive(client.inputBuffer, error)) {
        error.AppendContext("Failed to receive data from client");
        return;
    }
    if (error.IsSet()) return;

    // Process received messages
    while (client.inputBuffer.GetRemainingSize() >= 12) { // Minimum message size (header)
        Message message;
        if (!message.Deserialize(client.inputBuffer, error)) {
            error.AppendContext("Failed to deserialize message from client");
            return;
        }
        if (error.IsSet()) return;

        // Handle HelloCommand (ModuleId=1, CommandId=1)
        if (message.GetModuleId() == 1 && message.GetCommandId() == 1) {
            Buffer responseBuffer(4096);
            const std::vector<uint8_t>& body = message.GetBody();
            responseBuffer.Write(body.data(), body.size(), error);
            if (error.IsSet()) {
                error.AppendContext("Failed to write HelloCommand body to buffer");
                return;
            }

            uint32_t roleId;
            if (!responseBuffer.Read(roleId, message.GetEndian(), error)) {
                error.AppendContext("Failed to read RoleId from HelloCommand");
                return;
            }
            if (error.IsSet()) return;

            client.roleId = roleId;
            std::cout << "Client sent HelloCommand, RoleId: " << roleId << std::endl;

            // Echo the roleId back as a response
            Buffer output(4096);
            output.Write(roleId, Message::Endian::Native, error);
            if (error.IsSet()) {
                error.AppendContext("Failed to write RoleId response");
                return;
            }

            Message response;
            response.SetModuleId(1);
            response.SetCommandId(1);
            response.SetSize(12 + output.GetSize());
            response.SetCrc(0); // Placeholder

            Buffer responseMessageBuffer(4096);
            if (!response.Serialize(responseMessageBuffer, error)) {
                error.AppendContext("Failed to serialize HelloCommand response");
                return;
            }
            if (error.IsSet()) return;

            client.outputBuffer = std::move(responseMessageBuffer);
            client.pendingOutput = true;
        }
        // Add more message handling as needed (e.g., GetFileList)
    }

    error.SetError(ErrorCode::Success, "");
}

void Server::Send(ClientData& client, Error& error) {
    if (!client.socket->Send(client.outputBuffer, error)) {
        error.AppendContext("Failed to send data to client");
        return;
    }
    if (error.IsSet()) return;

    client.pendingOutput = false;
    error.SetError(ErrorCode::Success, "");
}
