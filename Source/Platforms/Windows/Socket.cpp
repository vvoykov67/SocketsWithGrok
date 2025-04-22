#include "Socket.h"
#include "IpAddress.h"
#include "Buffer.h"
#include "WinsockInitializer.h"
#include <iostream>

Socket::Socket() : socketHandle(INVALID_SOCKET), isInitialized(false) {}

Socket::~Socket() {
    Close();
}

bool Socket::CreateSocket(Error& error) {
    if (isInitialized) {
        error.SetError(ErrorCode::SocketCreateFailed, "Socket already initialized");
        return false;
    }

    socketHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socketHandle == INVALID_SOCKET) {
        error.SetError(ErrorCode::SocketCreateFailed, "Failed to create socket: " + std::to_string(WSAGetLastError()));
        return false;
    }

    isInitialized = true;
    error.SetError(ErrorCode::Success, "");
    return true;
}

bool Socket::Bind(const IpAddress& address, uint16_t port, Error& error) {
    if (!isInitialized) {
        error.SetError(ErrorCode::SocketInvalidHandle, "Socket not initialized");
        return false;
    }

    sockaddr_in addr = address.ToSockAddr(port);
    if (bind(socketHandle, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0) {
        error.SetError(ErrorCode::SocketListenerBindListenFailed, "Failed to bind socket: " + std::to_string(WSAGetLastError()));
        return false;
    }

    error.SetError(ErrorCode::Success, "");
    return true;
}

bool Socket::Listen(int backlog, Error& error) {
    if (!isInitialized) {
        error.SetError(ErrorCode::SocketInvalidHandle, "Socket not initialized");
        return false;
    }

    if (listen(socketHandle, backlog) != 0) {
        error.SetError(ErrorCode::SocketListenerBindListenFailed, "Failed to listen on socket: " + std::to_string(WSAGetLastError()));
        return false;
    }

    error.SetError(ErrorCode::Success, "");
    return true;
}

std::unique_ptr<Socket> Socket::Accept(Error& error) {
    if (!isInitialized) {
        error.SetError(ErrorCode::SocketInvalidHandle, "Socket not initialized");
        return nullptr;
    }

    sockaddr_in clientAddr;
    int addrLen = sizeof(clientAddr);
    SOCKET clientHandle = accept(socketHandle, reinterpret_cast<sockaddr*>(&clientAddr), &addrLen);
    if (clientHandle == INVALID_SOCKET) {
        int err = WSAGetLastError();
        if (err == WSAEWOULDBLOCK) {
            error.SetError(ErrorCode::Success, "No connections available");
            return nullptr;
        }
        error.SetError(ErrorCode::SocketAcceptFailed, "Failed to accept connection: " + std::to_string(err));
        return nullptr;
    }

    auto clientSocket = std::make_unique<Socket>();
    clientSocket->socketHandle = clientHandle;
    clientSocket->isInitialized = true;
    error.SetError(ErrorCode::Success, "");
    return clientSocket;
}

bool Socket::Connect(const IpAddress& address, uint16_t port, Error& error) {
    if (!isInitialized) {
        error.SetError(ErrorCode::SocketInvalidHandle, "Socket not initialized");
        return false;
    }

    sockaddr_in addr = address.ToSockAddr(port);
    if (connect(socketHandle, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0) {
        int err = WSAGetLastError();
        if (err != WSAEWOULDBLOCK) {
            error.SetError(ErrorCode::SocketConnectFailed, "Failed to connect: " + std::to_string(err));
            return false;
        }
        // Non-blocking connect may return WSAEWOULDBLOCK, check with select
        fd_set writeFds;
        FD_ZERO(&writeFds);
        FD_SET(socketHandle, &writeFds);
        struct timeval timeout = {5, 0}; // 5-second timeout
        if (select(static_cast<int>(socketHandle) + 1, nullptr, &writeFds, nullptr, &timeout) <= 0) {
            error.SetError(ErrorCode::SocketConnectFailed, "Connect timeout or error");
            return false;
        }
        int soError;
        int len = sizeof(soError);
        if (getsockopt(socketHandle, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&soError), &len) != 0 || soError != 0) {
            error.SetError(ErrorCode::SocketConnectFailed, "Connect failed: " + std::to_string(soError));
            return false;
        }
    }

    error.SetError(ErrorCode::Success, "");
    return true;
}

bool Socket::Receive(Buffer& buffer, Error& error) {
    if (!isInitialized) {
        error.SetError(ErrorCode::SocketInvalidHandle, "Socket not initialized");
        return false;
    }

    uint32_t remainingCapacity;
    uint8_t* writePtr = buffer.GetWritePointer(remainingCapacity);
    if (writePtr == nullptr || remainingCapacity == 0) {
        error.SetError(ErrorCode::BufferWriteRawFailed, "Buffer not initialized or no remaining capacity");
        return false;
    }

    int bytesReceived = recv(socketHandle, reinterpret_cast<char*>(writePtr), remainingCapacity, 0);
    if (bytesReceived < 0) {
        int err = WSAGetLastError();
        if (err == WSAEWOULDBLOCK) {
            error.SetError(ErrorCode::Success, "No data available");
            return true;
        }
        error.SetError(ErrorCode::SocketReceiveFailed, "Receive failed: " + std::to_string(err));
        return false;
    }
    if (bytesReceived == 0) {
        error.SetError(ErrorCode::SocketReceiveFailed, "Connection closed");
        return false;
    }

    buffer.AdvanceWrite(bytesReceived);
    error.SetError(ErrorCode::Success, "");
    return true;
}

bool Socket::Send(Buffer& buffer, Error& error) {
    if (!isInitialized) {
        error.SetError(ErrorCode::SocketInvalidHandle, "Socket not initialized");
        return false;
    }

    uint32_t remainingSize;
    const uint8_t* readPtr = buffer.GetReadPointer(remainingSize);
    if (readPtr == nullptr || remainingSize == 0) {
        error.SetError(ErrorCode::BufferReadRawNoData, "Buffer not initialized or no data to send");
        return false;
    }

    int bytesSent = send(socketHandle, reinterpret_cast<char*>(const_cast<uint8_t*>(readPtr)), remainingSize, 0);
    if (bytesSent < 0) {
        int err = WSAGetLastError();
        if (err == WSAEWOULDBLOCK) {
            error.SetError(ErrorCode::Success, "Send blocked");
            return true;
        }
        error.SetError(ErrorCode::SocketSendFailed, "Send failed: " + std::to_string(err));
        return false;
    }

    buffer.AdvanceRead(bytesSent);
    error.SetError(ErrorCode::Success, "");
    return true;
}

bool Socket::SetNonBlocking(Error& error) {
    if (!isInitialized) {
        error.SetError(ErrorCode::SocketInvalidHandle, "Socket not initialized");
        return false;
    }

    u_long mode = 1;
    if (ioctlsocket(socketHandle, FIONBIO, &mode) != 0) {
        error.SetError(ErrorCode::SocketSetOptionFailed, "Failed to set non-blocking mode: " + std::to_string(WSAGetLastError()));
        return false;
    }

    error.SetError(ErrorCode::Success, "");
    return true;
}

bool Socket::SetReuseAddress(Error& error) {
    if (!isInitialized) {
        error.SetError(ErrorCode::SocketInvalidHandle, "Socket not initialized");
        return false;
    }

    int opt = 1;
    if (setsockopt(socketHandle, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&opt), sizeof(opt)) != 0) {
        error.SetError(ErrorCode::SocketSetOptionFailed, "Failed to set SO_REUSEADDR: " + std::to_string(WSAGetLastError()));
        return false;
    }

    error.SetError(ErrorCode::Success, "");
    return true;
}

void Socket::Close() {
    if (isInitialized) {
        closesocket(socketHandle);
        socketHandle = INVALID_SOCKET;
        isInitialized = false;
    }
}

int Socket::GetHandle() const {
    return static_cast<int>(socketHandle);
}
