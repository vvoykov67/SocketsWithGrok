#include "DiskFile.h"
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <cerrno>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(dir) _mkdir(dir.c_str())
#else
#include <unistd.h>
#define MKDIR(dir) mkdir(dir.c_str(), 0755)
#endif

DiskFile::DiskFile() {}

DiskFile::~DiskFile() {
    Close();
}

bool DiskFile::Open(const std::string& path, bool append) {
    filePath = path;
    std::ios::openmode mode = std::ios::out;
    if (append) mode |= std::ios::app;
    file.open(path, mode);
    return file.is_open();
}

void DiskFile::WriteLine(const std::string& line) {
    if (file.is_open()) {
        file << line << std::endl;
    }
}

void DiskFile::Flush() {
    if (file.is_open()) {
        file.flush();
    }
}

void DiskFile::Close() {
    if (file.is_open()) {
        file.close();
    }
}

bool DiskFile::Rename(const std::string& newPath) {
    Close();
    return std::rename(filePath.c_str(), newPath.c_str()) == 0;
}

bool DiskFile::Remove(const std::string& path) {
    return std::remove(path.c_str()) == 0;
}

size_t DiskFile::GetSize() {
    if (!file.is_open()) return 0;
    file.flush();
    std::ifstream in(filePath, std::ios::binary | std::ios::ate);
    return static_cast<size_t>(in.tellg());
}

bool DiskFile::CreateDirectory(const std::string& dirPath) {
    return MKDIR(dirPath) == 0 || errno == EEXIST;
}

std::string DiskFile::ReadAllText(const std::string& path) {
    std::ifstream file(path);
    if (!file) return {};
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

std::vector<std::string> DiskFile::ReadLines(const std::string& path) {
    std::ifstream file(path);
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    return lines;
}
