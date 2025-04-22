#include "Logger.h"
#include "Server.h"
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
    Server server;
    IpAddress address("127.0.0.1"); // Listen on all interfaces
    uint16_t port = 4445;
    Error error;

    WinsockInitializer::Initialize(error);
    if (!server.Initialize(address, port, error)) {
        std::cerr << "Server init failed: " << error.GetText() << std::endl;
        return 1;
    }

    std::cout << "Server running on port " << port << std::endl;
    server.Run();

    LOG_INFO("Application exiting");
    return EXIT_SUCCESS;
}
