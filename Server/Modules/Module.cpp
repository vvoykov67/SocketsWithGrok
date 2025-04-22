#include "Module.h"

Module::Module(uint16_t moduleId) : moduleId(moduleId) {}

uint16_t Module::GetModuleId() const {
    return moduleId;
}

void Module::RegisterCommand(std::unique_ptr<Command> command) {
    commands[command->GetCommandId()] = std::move(command);
}

const Command* Module::FindCommand(uint16_t commandId) const {
    auto it = commands.find(commandId);
    return it != commands.end() ? it->second.get() : nullptr;
}
