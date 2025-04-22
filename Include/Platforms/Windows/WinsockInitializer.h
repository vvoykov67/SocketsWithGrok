#pragma once

#include <winsock2.h>

class Error; // Forward declaration

class WinsockInitializer
{
public:
    static WinsockInitializer& GetInstance();
    static void Initialize(Error& error);

private:
    WinsockInitializer();
    ~WinsockInitializer();

    bool Initialized;
    WSADATA WsaData;
};
