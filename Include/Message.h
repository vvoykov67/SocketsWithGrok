#pragma once

#include <cstdint>

class Buffer; // Forward declaration for Buffer class
class Error;  // Forward declaration for Error class
#include <vector>

class Message {
public:
    // Endianness for serialization/deserialization
    enum class Endian {
        Native,  // Use system-native endianness
        Big,     // Big-endian (network byte order)
        Little   // Little-endian
    };

    Message();
    ~Message() = default;

    bool Serialize(Buffer& buffer, Error& error) const;
    bool Deserialize(Buffer& buffer, Error& error);

    uint32_t GetSize() const;
    uint32_t GetCrc() const;
    uint16_t GetModuleId() const;
    uint16_t GetCommandId() const;
    const std::vector<uint8_t>& GetBody() const;
    Endian GetEndian() const;

    void SetSize(uint32_t size);
    void SetCrc(uint32_t crc);
    void SetModuleId(uint16_t moduleId);
    void SetCommandId(uint16_t commandId);
    void SetEndian(Endian endian);

private:
    uint32_t size;
    uint32_t crc;
    uint16_t moduleId;
    uint16_t commandId;
    std::vector<uint8_t> body;
    Endian endian;
};
