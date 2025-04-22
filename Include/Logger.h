#pragma once

#include "DiskFile.h"
#include <string>
#include <mutex>

enum class LogLevel {
    INFO,
    WARNING,
    ERROR,
    FATAL
};

class Logger {
public:
    static Logger& Instance();

    void LoadConfiguration(const std::string& configPath);
    void Log(LogLevel level, const char* format, ...);
    LogLevel GetCurrentLevel() const;

private:
    Logger();
    void RotateIfNeeded();
    void OpenLogFile();
    void RotateFiles();
    std::string LevelToString(LogLevel level);
    std::string GetThreadIdString();

    std::mutex Mutex;
    DiskFile LogFile;

    LogLevel CurrentLevel;
    std::string Directory;
    std::string BaseFileName;
    size_t MaxFileSize;
    size_t MaxFiles;
};

#pragma once
#include "Logger.h"

#define LOG_INFO(fmt, ...) \
    do { if (Logger::Instance().GetCurrentLevel() <= LogLevel::INFO) \
        Logger::Instance().Log(LogLevel::INFO, fmt, ##__VA_ARGS__); } while (0)

#define LOG_WARNING(fmt, ...) \
    do { if (Logger::Instance().GetCurrentLevel() <= LogLevel::WARNING) \
        Logger::Instance().Log(LogLevel::WARNING, fmt, ##__VA_ARGS__); } while (0)

#define LOG_ERROR(fmt, ...) \
    do { if (Logger::Instance().GetCurrentLevel() <= LogLevel::ERROR) \
        Logger::Instance().Log(LogLevel::ERROR, fmt, ##__VA_ARGS__); } while (0)

#define LOG_FATAL(fmt, ...) \
    do { if (Logger::Instance().GetCurrentLevel() <= LogLevel::FATAL) \
        Logger::Instance().Log(LogLevel::FATAL, fmt, ##__VA_ARGS__); } while (0)
