#pragma once

#include "Error.h"
#include <winsock2.h>

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
