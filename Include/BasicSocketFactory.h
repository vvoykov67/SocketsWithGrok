#pragma once

class BasicSocket; // Forward declaration
class Error;       // Forward declaration
class IpAddress;   // Forward declaration

class BasicSocketFactory
{
public:
    static BasicSocketFactory* Instance;
    virtual BasicSocket* CreateSocket(const IpAddress& ipAddress, Error& error) = 0;
    virtual ~BasicSocketFactory() = default;
};
