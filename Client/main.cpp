#include "Logger.h"
#include "Client.h"
#include "IpAddress.h"
#include "WinsockInitializer.h"
#include <iostream>

int main() {
    // Load logger configuration
    try {
        Logger::Instance().LoadConfiguration("Config.Properties");
        LOG_WARNING("Logger initialized successfully");
    } catch (const std::exception& ex) {
        std::cerr << "Failed to initialize logger: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    // Application logic starts here
    LOG_INFO("Application started");
    
    Client client;
    IpAddress address("127.0.0.1"); // Server address
    uint16_t port = 4445;
    Error error;

    WinsockInitializer::Initialize(error);
    if (!client.Connect(address, port, error)) {
        std::cerr << "Connect failed: " << error.GetText() << std::endl;
        return 1;
    }

    // Send HelloCommand
    if (!client.SendHelloCommand(RoleId::Guest, error)) {
        std::cerr << "Send HelloCommand failed: " << error.GetText() << std::endl;
        return 1;
    }

    // Send GetFileList command
    if (!client.SendGetFileListCommand("/home/user", error)) {
        std::cerr << "Send GetFileList failed: " << error.GetText() << std::endl;
        return 1;
    }

    std::cout << "Client running, press Ctrl+C to stop" << std::endl;
    client.Run();

    LOG_INFO("Application exiting");
    return EXIT_SUCCESS;
}
