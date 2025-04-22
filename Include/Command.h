#pragma once

#include "Message.h" // For Buffer
#include "Error.h"   // For Error and ErrorCode
#include <cstdint>

class Command {
public:
    Command(uint16_t commandId);
    virtual ~Command() = default;

    uint16_t GetCommandId() const;

    // Process the command: deserialize input, execute, serialize output
    virtual bool Process(const Buffer& input, Buffer& output, Error& error) const = 0;

private:
    uint16_t commandId;
};
