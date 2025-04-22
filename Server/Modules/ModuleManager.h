#pragma once

#include "Message.h"
#include "Module.h"
#include "HelloModule.h"
#include "ProcessModule.h"
#include "FileModule.h"
#include "RoleId.h"
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

class ModuleManager {
public:
    ModuleManager();

    // Register a module
    void RegisterModule(std::unique_ptr<Module> module);

    // Initialize session with RoleID (called after HelloCommand)
    bool InitializeSession(uint32_t sessionId, RoleId roleId, Error& error);

    // Process a message for a session
    bool ProcessMessage(uint32_t sessionId, const Message& message, Buffer& output, Error& error);

private:
    std::unordered_map<uint16_t, std::unique_ptr<Module>> modules;
    std::unordered_map<uint32_t, std::vector<uint16_t>> sessionModules; // Session ID -> allowed ModuleIds
};
