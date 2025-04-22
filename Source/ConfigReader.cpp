#include "ConfigReader.h"
#include "DiskFile.h"
#include <algorithm>

static std::string Trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

std::map<std::string, std::string> ConfigReader::Parse(const std::string& path) {
    auto lines = DiskFile::ReadLines(path);
    std::map<std::string, std::string> config;

    for (const auto& line : lines) {
        std::string trimmed = Trim(line);
        if (trimmed.empty() || trimmed[0] == '#') continue;

        size_t eq = trimmed.find('=');
        if (eq != std::string::npos) {
            std::string key = Trim(trimmed.substr(0, eq));
            std::string value = Trim(trimmed.substr(eq + 1));
            config[key] = value;
        }
    }

    return config;
}
