#include "Buffer.h"
#include <cstring>

Buffer::Buffer(uint32_t capacity) 
    : data(capacity), writePosition(0), readPosition(0), isInitialized(true) {}

Buffer::~Buffer() {
    Clear();
}

template<typename T>
bool Buffer::Write(const T& value, Message::Endian endian, Error& error) {
    if (!isInitialized) {
        error.SetError(ErrorCode::BufferAlreadyInitialized, "Buffer not initialized");
        return false;
    }
    if (writePosition + sizeof(T) > data.size()) {
        error.SetError(ErrorCode::BufferWriteRawFailed, "Insufficient buffer capacity");
        return false;
    }

    T writeValue = value;
    if (endian == Message::Endian::Big) {
        writeValue = ReverseBytes(value);
    } else if (endian == Message::Endian::Little) {
        // Native is assumed little-endian for simplicity; adjust if needed
        // If native is big-endian, reverse for little-endian
        writeValue = ReverseBytes(value);
    }

    std::memcpy(data.data() + writePosition, &writeValue, sizeof(T));
    writePosition += sizeof(T);
    error.SetError(ErrorCode::Success, "");
    return true;
}

template<typename T>
bool Buffer::Read(T& value, Message::Endian endian, Error& error) {
    if (!isInitialized) {
        error.SetError(ErrorCode::BufferAlreadyInitialized, "Buffer not initialized");
        return false;
    }
    if (readPosition + sizeof(T) > writePosition) {
        error.SetError(ErrorCode::BufferInsufficientData, "Insufficient data for read");
        return false;
    }

    std::memcpy(&value, data.data() + readPosition, sizeof(T));
    if (endian == Message::Endian::Big) {
        value = ReverseBytes(value);
    } else if (endian == Message::Endian::Little) {
        // Native is assumed little-endian; adjust if needed
        value = ReverseBytes(value);
    }

    readPosition += sizeof(T);
    error.SetError(ErrorCode::Success, "");
    return true;
}

bool Buffer::Write(const uint8_t* data, uint32_t size, Error& error) {
    if (!isInitialized) {
        error.SetError(ErrorCode::BufferAlreadyInitialized, "Buffer not initialized");
        return false;
    }
    if (writePosition + size > this->data.size()) {
        error.SetError(ErrorCode::BufferWriteRawFailed, "Insufficient buffer capacity");
        return false;
    }

    std::memcpy(this->data.data() + writePosition, data, size);
    writePosition += size;
    error.SetError(ErrorCode::Success, "");
    return true;
}

bool Buffer::Read(uint8_t* data, uint32_t size, Error& error) {
    if (!isInitialized) {
        error.SetError(ErrorCode::BufferAlreadyInitialized, "Buffer not initialized");
        return false;
    }
    if (readPosition + size > writePosition) {
        error.SetError(ErrorCode::BufferInsufficientData, "Insufficient data for read");
        return false;
    }

    std::memcpy(data, this->data.data() + readPosition, size);
    readPosition += size;
    error.SetError(ErrorCode::Success, "");
    return true;
}

uint8_t* Buffer::GetWritePointer(uint32_t& remainingCapacity) {
    if (!isInitialized) {
        remainingCapacity = 0;
        return nullptr;
    }
    remainingCapacity = static_cast<uint32_t>(data.size()) - writePosition;
    return data.data() + writePosition;
}

void Buffer::AdvanceWrite(uint32_t size) {
    if (writePosition + size <= data.size()) {
        writePosition += size;
    }
}

const uint8_t* Buffer::GetReadPointer(uint32_t& remainingSize) const {
    if (!isInitialized) {
        remainingSize = 0;
        return nullptr;
    }
    remainingSize = writePosition - readPosition;
    return data.data() + readPosition;
}

void Buffer::AdvanceRead(uint32_t size) {
    if (readPosition + size <= writePosition) {
        readPosition += size;
    }
}

template<typename T>
T Buffer::ReverseBytes(T value) const {
    T reversed = 0;
    uint8_t* bytes = reinterpret_cast<uint8_t*>(&value);
    for (size_t i = 0; i < sizeof(T); ++i) {
        reversed = (reversed << 8) | bytes[i];
    }
    return reversed;
}

uint32_t Buffer::GetSize() const {
    return writePosition;
}

uint32_t Buffer::GetRemainingSize() const {
    return writePosition - readPosition;
}

uint32_t Buffer::GetCapacity() const {
    return static_cast<uint32_t>(data.size());
}

void Buffer::Clear() {
    writePosition = 0;
    readPosition = 0;
    data.assign(data.size(), 0);
}

// Explicit template instantiations for supported types
template bool Buffer::Write<uint32_t>(const uint32_t& value, Message::Endian endian, Error& error);
template bool Buffer::Read<uint32_t>(uint32_t& value, Message::Endian endian, Error& error);
template bool Buffer::Write<uint16_t>(const uint16_t& value, Message::Endian endian, Error& error);
template bool Buffer::Read<uint16_t>(uint16_t& value, Message::Endian endian, Error& error);
