#pragma once

#include <string>

typedef enum _ErrorCode
{
    Success = 0,                    // No error

    // Buffer errors
    BufferAlreadyInitialized = 100, // Buffer initialization already completed
    BufferUnsupportedTypeWrite = 101, // Type not supported for buffer write
    BufferUnsupportedTypeRead = 102, // Type not supported for buffer read
    BufferWriteRawFailed = 103,     // Unable to write raw data to buffer
    BufferReadRawNoData = 104,      // No data available to read from buffer
    BufferInsufficientData = 105,    // Insufficient data in buffer for read

    // Message errors
    MessageSerializeFailed = 200,   // Unable to serialize message
    MessageDeserializeFailed = 201, // Unable to deserialize message

    // IpAddress errors
    IpAddressInvalid = 300,         // Invalid IP address format

    // Socket errors
    SocketAddressFamilyMismatch = 400, // Mismatched address family
    SocketInvalidAddress = 401,     // Invalid address provided
    SocketConnectFailed = 402,      // Failed to establish connection
    SocketBindFailed = 403,         // Failed to bind socket
    SocketListenFailed = 404,       // Failed to listen on socket
    SocketAcceptFailed = 405,       // Failed to accept connection
    SocketSendFailed = 406,         // Failed to send data
    SocketReceiveFailed = 407,      // Failed to receive data
    SocketCreateFailed = 408,       // Failed to create socket
    SocketSetOptionFailed = 409,    // Failed to set socket option
    SocketInvalidHandle = 410,      // Invalid socket handle

    // SocketFactory errors
    SocketFactoryCreateFailed = 500, // Unable to create socket instance

    // SocketListener errors
    SocketListenerBindListenFailed = 600, // Unable to bind or listen on socket

    // Client errors
    ClientCreateSocketFailed = 700, // Unable to create client socket
    ClientSerializeFailed = 701,    // Unable to serialize client message
    ClientReceiveFailed = 702,      // Unable to receive client message

    // File errors
    FileAddFileNonDirectory = 800,  // Cannot add file to non-directory
    FileAddDirectoryNonDirectory = 801, // Cannot add directory to non-directory
    FileSerializeMetadataFailed = 802, // Failed to serialize file metadata
    FileSerializeSizeFailed = 803,  // Failed to serialize file size
    FileSerializeBlobFlagFailed = 804, // Failed to serialize blob flag
    FileSerializeBlobFailed = 805,  // Failed to serialize blob data
    FileSerializeChildCountFailed = 806, // Failed to serialize child count
    FileReadTypeFailed = 807,       // Failed to read file type
    FileReadNameFailed = 808,       // Failed to read file name
    FileReadSizeFailed = 809,       // Failed to read file size
    FileReadBlobFlagFailed = 810,   // Failed to read blob flag
    FileReadBlobSizeFailed = 811,   // Failed to read blob size
    FileReadBlobFailed = 812,       // Failed to read blob data
    FileReadChildCountFailed = 813, // Failed to read child count

    // List errors
    ListSerializeSizeFailed = 900,  // Failed to serialize list size
    ListDeserializeSizeFailed = 901, // Failed to deserialize list size

    // WinsockInitializer errors
    WinsockNotInitialized = 1000,   // Winsock initialization not completed
    WinsockStartupFailed = 1001     // Failed to start Winsock
} ErrorCode;

class Error {
public:
    Error();

    // Set the error with a code and optional message
    void SetError(ErrorCode code, const std::string& text);

    // Append additional context to the existing error message (if set)
    void AppendContext(const std::string& context);

    // Clear the error state
    void Clear();

    // Get the error code
    int GetCode() const;

    // Get the error message
    const std::string& GetText() const;

    // Check if an error is set
    bool IsSet() const;

private:
    ErrorCode code;
    std::string text;
};
