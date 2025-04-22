#pragma once

#include "Command.h"
#include <cstdint>
#include <memory>
#include <unordered_map>

class Module {
public:
    Module(uint16_t moduleId);
    virtual ~Module() = default;

    uint16_t GetModuleId() const;

    // Register a command in the module
    void RegisterCommand(std::unique_ptr<Command> command);

    // Find a command by ID
    const Command* FindCommand(uint16_t commandId) const;

private:
    uint16_t moduleId;
    std::unordered_map<uint16_t, std::unique_ptr<Command>> commands;
};
