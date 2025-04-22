#pragma once

#include "Error.h"
#include <string>

struct sockaddr_in;

class IpAddress {
public:
    IpAddress();
    IpAddress(const std::string& address);

    // Set the IP address (validates format)
    bool SetAddress(const std::string& address, Error& error);

    // Get the IP address as a string
    std::string GetAddress() const;

    // Convert to sockaddr_in for socket operations
    sockaddr_in ToSockAddr(uint16_t port) const;

private:
    std::string address;
};
