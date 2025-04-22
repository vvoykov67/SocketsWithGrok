#pragma once

#include "Error.h"
#include "Message.h"
#include <cstdint>
#include <vector>

class Buffer {
public:
    explicit Buffer(uint32_t capacity);
    ~Buffer();

    // Write a value of type T (e.g., uint32_t, uint16_t)
    template<typename T>
    bool Write(const T& value, Message::Endian endian, Error& error);

    // Read a value of type T (e.g., uint32_t, uint16_t)
    template<typename T>
    bool Read(T& value, Message::Endian endian, Error& error);

    // Write raw bytes
    bool Write(const uint8_t* data, uint32_t size, Error& error);

    // Read raw bytes
    bool Read(uint8_t* data, uint32_t size, Error& error);

    // Get a pointer to the write position and remaining capacity
    uint8_t* GetWritePointer(uint32_t& remainingCapacity);

    // Advance the write position after writing
    void AdvanceWrite(uint32_t size);

    // Get a pointer to the read position and remaining data size
    const uint8_t* GetReadPointer(uint32_t& remainingSize) const;

    // Advance the read position after reading
    void AdvanceRead(uint32_t size);

    // Get total size of data in buffer
    uint32_t GetSize() const;

    // Get remaining data size (from read position)
    uint32_t GetRemainingSize() const;

    // Get buffer capacity
    uint32_t GetCapacity() const;

    // Clear the buffer (reset read/write positions)
    void Clear();

private:
    // Helper to reverse bytes for endian conversion
    template<typename T>
    T ReverseBytes(T value) const;

    std::vector<uint8_t> data;
    uint32_t writePosition;
    uint32_t readPosition;
    bool isInitialized;
};
