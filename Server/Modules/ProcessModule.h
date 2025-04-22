#pragma once

#include "Module.h"

class ProcessModule : public Module {
public:
    ProcessModule();

    class CreateProcessCommand : public Command {
    public:
        CreateProcessCommand();
        bool Process(const Buffer& input, Buffer& output, Error& error) const override;
    };

    class GetProcessListCommand : public Command {
    public:
        GetProcessListCommand();
        bool Process(const Buffer& input, Buffer& output, Error& error) const override;
    };

    class KillProcessCommand : public Command {
    public:
        KillProcessCommand();
        bool Process(const Buffer& input, Buffer& output, Error& error) const override;
    };
};
