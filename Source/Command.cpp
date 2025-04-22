#include "Command.h"

Command::Command(uint16_t commandId) : commandId(commandId) {}

uint16_t Command::GetCommandId() const {
    return commandId;
}
