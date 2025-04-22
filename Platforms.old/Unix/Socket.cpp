#include "Socket.h"
#include <stdexcept>
#include <unistd.h>

Socket::Socket(const IpAddress& ipAddress)
    : SocketHandle(-1)
    , IpAddress(ipAddress)
{
    SocketHandle = socket(IpAddress.GetAddressFamily(), SOCK_STREAM, 0);
    if (SocketHandle < 0)
    {
        throw std::runtime_error("Failed to create socket");
    }
}

Socket::Socket(int sock)
    : SocketHandle(sock)
    , IpAddress("0.0.0.0", 0, Error())
{
}

Socket::~Socket()
{
    Close();
}

bool Socket::Connect(const IpAddress& address, Error& error)
{
    if (address.GetAddressFamily() != IpAddress.GetAddressFamily())
    {
        error.SetError(ErrorCode::SocketAddressFamilyMismatch, "Address family mismatch");
        return false;
    }

    int result = ::connect(SocketHandle, reinterpret_cast<const sockaddr*>(&address.GetAddress()), address.GetAddressLength());
    if (result < 0)
    {
        error.SetError(ErrorCode::SocketConnectFailed, "Failed to connect");
        return false;
    }
    return true;
}

bool Socket::Bind(const IpAddress& address, Error& error)
{
    if (address.GetAddressFamily() != IpAddress.GetAddressFamily())
    {
        error.SetError(ErrorCode::SocketAddressFamilyMismatch, "Address family mismatch");
        return false;
    }

    int result = ::bind(SocketHandle, reinterpret_cast<const sockaddr*>(&address.GetAddress()), address.GetAddressLength());
    if (result < 0)
    {
        error.SetError(ErrorCode::SocketBindFailed, "Failed to bind");
        return false;
    }
    return true;
}

bool Socket::Listen(int backlog, Error& error)
{
    int result = ::listen(SocketHandle, backlog);
    if (result < 0)
    {
        error.SetError(ErrorCode::SocketListenFailed, "Failed to listen");
        return false;
    }
    return true;
}

bool Socket::Accept(BasicSocket& socket, Error& error)
{
    sockaddr_storage addr = {};
    socklen_t addrLen = sizeof(sockaddr_storage);
    int client = ::accept(SocketHandle, reinterpret_cast<sockaddr*>(&addr), &addrLen);
    if (client < 0)
    {
        error.SetError(ErrorCode::SocketAcceptFailed, "Failed to accept connection");
        return false;
    }
    socket = Socket(client);
    return true;
}

bool Socket::Send(const Buffer& buffer, Error& error)
{
    const char* data = buffer.GetData();
    size_t size = buffer.Size();
    int sent = ::send(SocketHandle, data, size, 0);
    if (sent != size)
    {
        error.SetError(ErrorCode::SocketSendFailed, "Failed to send data");
        return false;
    }
    return true;
}

bool Socket::Receive(Buffer& buffer, size_t maxLen, Error& error)
{
    char temp[1024];
    size_t toRead = (std::min)(maxLen, sizeof(temp));
    int received = ::recv(SocketHandle, temp, toRead, 0);
    if (received <= 0)
    {
        error.SetError(ErrorCode::SocketReceiveFailed, "Failed to receive data");
        return false;
    }
    return buffer.WriteRaw(temp, received, error);
}

void Socket::Close()
{
    if (SocketHandle >= 0)
    {
        ::close(SocketHandle);
        SocketHandle = -1;
    }
}
