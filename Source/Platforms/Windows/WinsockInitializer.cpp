#include "WinsockInitializer.h"
#include "Error.h"

WinsockInitializer& WinsockInitializer::GetInstance()
{
    static WinsockInitializer instance;
    return instance;
}

void WinsockInitializer::Initialize(Error& error)
{
    WinsockInitializer& instance = GetInstance();
    if (!instance.Initialized)
    {
        error.SetError(ErrorCode::WinsockNotInitialized, "Winsock not initialized");
    }
}

WinsockInitializer::WinsockInitializer()
    : Initialized(false)
{
    int result = WSAStartup(MAKEWORD(2, 2), &WsaData);
    if (result != 0)
    {
        return; // Error handled by caller via Initialize
    }
    Initialized = true;
}

WinsockInitializer::~WinsockInitializer()
{
    if (Initialized)
    {
        WSACleanup();
    }
}
