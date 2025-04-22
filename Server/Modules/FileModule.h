#pragma once

#include "Module.h"

class FileModule : public Module {
public:
    FileModule();

    class GetFileListCommand : public Command {
    public:
        GetFileListCommand();
        bool Process(const Buffer& input, Buffer& output, Error& error) const override;
    };

    class OpenFileCommand : public Command {
    public:
        OpenFileCommand();
        bool Process(const Buffer& input, Buffer& output, Error& error) const override;
    };

    class TransferFileCommand : public Command {
    public:
        TransferFileCommand();
        bool Process(const Buffer& input, Buffer& output, Error& error) const override;
    };

    class ReadBlockCommand : public Command {
    public:
        ReadBlockCommand();
        bool Process(const Buffer& input, Buffer& output, Error& error) const override;
    };

    class WriteBlockCommand : public Command {
    public:
        WriteBlockCommand();
        bool Process(const Buffer& input, Buffer& output, Error& error) const override;
    };
};
