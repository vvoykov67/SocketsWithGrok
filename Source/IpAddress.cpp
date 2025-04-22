#include "IpAddress.h"
#if defined(_WIN32) || defined(_WIN64)
#include <winsock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <cstring>
#include <iostream>

IpAddress::IpAddress() : address("0.0.0.0") {}

IpAddress::IpAddress(const std::string& address) : address("0.0.0.0") {
    Error error;
    SetAddress(address, error);
    if (error.GetCode() != ErrorCode::Success) {
        std::cerr << "Invalid IP address: " << error.GetText() << std::endl;
    }
}

bool IpAddress::SetAddress(const std::string& address, Error& error) {
    in_addr addr;
    if (inet_pton(AF_INET, address.c_str(), &addr) != 1) {
        error.SetError(ErrorCode::IpAddressInvalid, "Invalid IPv4 address format: " + address);
        return false;
    }
    this->address = address;
    error.SetError(ErrorCode::Success, "");
    return true;
}

std::string IpAddress::GetAddress() const {
    return address;
}

sockaddr_in IpAddress::ToSockAddr(uint16_t port) const {
    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, address.c_str(), &addr.sin_addr);
    return addr;
}
