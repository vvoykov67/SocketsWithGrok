#pragma once

#include "Module.h"
#include "RoleId.h"

class HelloModule : public Module {
public:
    HelloModule();

    class HelloCommand : public Command {
    public:
        HelloCommand();
        bool Process(const Buffer& input, Buffer& output, Error& error) const override;
    };
};
