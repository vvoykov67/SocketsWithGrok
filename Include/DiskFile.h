#pragma once

#include <string>
#include <fstream>
#include <vector>

class DiskFile {
public:
    DiskFile();
    ~DiskFile();

    bool Open(const std::string& path, bool append = true);
    void WriteLine(const std::string& line);
    void Flush();
    void Close();

    bool Rename(const std::string& newPath);
    size_t GetSize();

    static bool Remove(const std::string& path);
    static bool CreateDirectory(const std::string& dirPath);
    static std::string ReadAllText(const std::string& path);
    static std::vector<std::string> ReadLines(const std::string& path);

private:
    std::ofstream file;
    std::string filePath;
};
