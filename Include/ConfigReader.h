#pragma once

#include <string>
#include <map>

class ConfigReader {
public:
    static std::map<std::string, std::string> Parse(const std::string& path);
};
