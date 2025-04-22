#include "ModuleManager.h"

ModuleManager::ModuleManager() {
    // Register default modules
    RegisterModule(std::make_unique<HelloModule>());
    RegisterModule(std::make_unique<ProcessModule>());
    RegisterModule(std::make_unique<FileModule>());
}

void ModuleManager::RegisterModule(std::unique_ptr<Module> module) {
    modules[module->GetModuleId()] = std::move(module);
}

bool ModuleManager::InitializeSession(uint32_t sessionId, RoleId roleId, Error& error) {
    std::vector<uint16_t> allowedModules;
    switch (roleId) {
        case RoleId::Guest:
            allowedModules = {1, 3}; // HelloModule, FileModule
            break;
        case RoleId::Admin:
            allowedModules = {1, 2, 3}; // HelloModule, ProcessModule, FileModule
            break;
        default:
            error.SetError(ErrorCode::BufferInsufficientData, "Invalid RoleID");
            return false;
    }
    sessionModules[sessionId] = allowedModules;
    error.SetError(ErrorCode::Success, "");
    return true;
}

bool ModuleManager::ProcessMessage(uint32_t sessionId, const Message& message, Buffer& output, Error& error) {
    // Check if module is allowed for this session
    auto sessionIt = sessionModules.find(sessionId);
    if (sessionIt == sessionModules.end() || 
        std::find(sessionIt->second.begin(), sessionIt->second.end(), message.GetModuleId()) == sessionIt->second.end()) {
        error.SetError(ErrorCode::BufferInsufficientData, "Unauthorized module access");
        return false;
    }

    // Find module
    auto moduleIt = modules.find(message.GetModuleId());
    if (moduleIt == modules.end()) {
        error.SetError(ErrorCode::BufferInsufficientData, "Invalid module ID");
        return false;
    }

    // Find command
    const Command* command = moduleIt->second->FindCommand(message.GetCommandId());
    if (!command) {
        error.SetError(ErrorCode::BufferInsufficientData, "Invalid command ID");
        return false;
    }

    // Deserialize body and process command
    Buffer input;
    std::vector<uint8_t> body;
    if (!message.Deserialize(input, body, error)) {
        error.SetError(ErrorCode::MessageDeserializeFailed, "Failed to deserialize message");
        return false;
    }
    input.WriteBytes(body.data(), body.size(), error);
    if (error.GetCode() != ErrorCode::Success) {
        error.SetError(ErrorCode::BufferWriteRawFailed, "Failed to write body to input buffer");
        return false;
    }
    return command->Process(input, output, error);
}
