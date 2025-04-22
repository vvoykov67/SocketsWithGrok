#include "SocketListener.h"
#include "Socket.h" // Client Socket class
#include "IpAddress.h"
#include "Socket.h"

SocketListener::SocketListener() {
    socket = std::make_unique<Socket>();
}

SocketListener::~SocketListener() {
    Close();
}

bool SocketListener::CreateSocket(Error& error) {
    if (!socket->CreateSocket(error)) {
        error.AppendContext("Failed to create socket");
        return false;
    }
    if (error.IsSet()) return false;

    if (!socket->SetNonBlocking(error)) {
        error.AppendContext("Failed to set non-blocking mode");
        socket->Close();
        return false;
    }
    if (error.IsSet()) return false;

    if (!socket->SetReuseAddress(error)) {
        error.AppendContext("Failed to set SO_REUSEADDR");
        socket->Close();
        return false;
    }
    if (error.IsSet()) return false;

    error.SetError(ErrorCode::Success, "");
    return true;
}

bool SocketListener::Bind(const IpAddress& address, uint16_t port, Error& error) {
    if (!socket->Bind(address, port, error)) {
        error.AppendContext("Failed to bind to address: " + address.GetAddress() + ":" + std::to_string(port));
        return false;
    }
    if (error.IsSet()) return false;

    error.SetError(ErrorCode::Success, "");
    return true;
}

bool SocketListener::Listen(int backlog, Error& error) {
    if (!socket->Listen(backlog, error)) {
        error.AppendContext("Failed to listen with backlog: " + std::to_string(backlog));
        return false;
    }
    if (error.IsSet()) return false;

    error.SetError(ErrorCode::Success, "");
    return true;
}

std::unique_ptr<Socket> SocketListener::Accept(Error& error) {
    auto clientSocket = socket->Accept(error);
    if (error.IsSet()) {
        error.AppendContext("Failed to accept client connection");
        return nullptr;
    }

    if (!clientSocket) {
        error.SetError(ErrorCode::Success, "No connections available");
        return nullptr;
    }

    error.SetError(ErrorCode::Success, "");
    return clientSocket;
}

int SocketListener::GetHandle() const {
    return socket->GetHandle();
}

void SocketListener::Close() {
    socket->Close();
}
