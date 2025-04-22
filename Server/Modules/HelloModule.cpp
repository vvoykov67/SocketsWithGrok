#include "HelloModule.h"

HelloModule::HelloModule() : Module(1) {
    RegisterCommand(std::make_unique<HelloCommand>());
}

HelloModule::HelloCommand::HelloCommand() : Command(1) {}

bool HelloModule::HelloCommand::Process(const Buffer& input, Buffer& output, Error& error) const {
    // Deserialize RoleID from input
    uint32_t roleIdRaw;
    if (!input.ReadUInt32(roleIdRaw, error)) {
        error.SetError(ErrorCode::BufferInsufficientData, "Failed to read RoleID");
        return false;
    }
    RoleId roleId = static_cast<RoleId>(roleIdRaw);
    if (roleId != RoleId::Guest && roleId != RoleId::Admin) {
        error.SetError(ErrorCode::BufferInsufficientData, "Invalid RoleID");
        return false;
    }

    // Serialize response (e.g., confirm role)
    if (!output.WriteUInt32(static_cast<uint32_t>(roleId), error)) {
        error.SetError(ErrorCode::BufferWriteRawFailed, "Failed to write RoleID response");
        return false;
    }
    error.SetError(ErrorCode::Success, "");
    return true;
}
