#include "Message.h"
#include "Buffer.h"
#include "Error.h"

Message::Message() 
    : size(0), crc(0), moduleId(0), commandId(0), body(), endian(Endian::Native) {}

bool Message::Serialize(Buffer& buffer, Error& error) const {
    // Write header fields
    if (!buffer.Write(size, endian, error) ||
        !buffer.Write(crc, endian, error) ||
        !buffer.Write(moduleId, endian, error) ||
        !buffer.Write(commandId, endian, error)) {
        error.SetError(ErrorCode::MessageSerializeFailed, "Failed to serialize message header");
        return false;
    }

    // Write body
    if (!buffer.Write(body.data(), body.size(), error)) {
        error.SetError(ErrorCode::MessageSerializeFailed, "Failed to serialize message body");
        return false;
    }

    error.SetError(ErrorCode::Success, "");
    return true;
}

bool Message::Deserialize(Buffer& buffer, Error& error) {
    // Check if buffer has enough data for the header
    if (buffer.GetRemainingSize() < 12) { // Size + CRC + ModuleId + CommandId = 4 + 4 + 2 + 2
        error.SetError(ErrorCode::BufferInsufficientData, "Insufficient data for message header");
        return false;
    }

    // Read header fields
    if (!buffer.Read(size, endian, error) ||
        !buffer.Read(crc, endian, error) ||
        !buffer.Read(moduleId, endian, error) ||
        !buffer.Read(commandId, endian, error)) {
        error.SetError(ErrorCode::MessageDeserializeFailed, "Failed to deserialize message header");
        return false;
    }

    // Validate size
    if (size < 12) {
        error.SetError(ErrorCode::MessageDeserializeFailed, "Invalid message size");
        return false;
    }
    uint32_t bodySize = size - 12;

    // Check if buffer has enough data for the body
    if (buffer.GetRemainingSize() < bodySize) {
        error.SetError(ErrorCode::BufferInsufficientData, "Insufficient data for message body");
        return false;
    }

    // Read body
    body.resize(bodySize);
    if (!buffer.Read(body.data(), bodySize, error)) {
        error.SetError(ErrorCode::MessageDeserializeFailed, "Failed to deserialize message body");
        return false;
    }

    error.SetError(ErrorCode::Success, "");
    return true;
}

uint32_t Message::GetSize() const {
    return size;
}

uint32_t Message::GetCrc() const {
    return crc;
}

uint16_t Message::GetModuleId() const {
    return moduleId;
}

uint16_t Message::GetCommandId() const {
    return commandId;
}

const std::vector<uint8_t>& Message::GetBody() const {
    return body;
}

Message::Endian Message::GetEndian() const {
    return endian;
}

void Message::SetSize(uint32_t size) {
    this->size = size;
}

void Message::SetCrc(uint32_t crc) {
    this->crc = crc;
}

void Message::SetModuleId(uint16_t moduleId) {
    this->moduleId = moduleId;
}

void Message::SetCommandId(uint16_t commandId) {
    this->commandId = commandId;
}

void Message::SetEndian(Endian endian) {
    this->endian = endian;
}
