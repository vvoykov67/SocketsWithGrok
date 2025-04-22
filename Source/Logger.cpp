#include "Logger.h"
#include "DiskFile.h"
#include "ConfigReader.h"

#include <ctime>
#include <sstream>
#include <iomanip>
#include <thread>
#include <cstdio>
#include <cstdarg>

Logger::Logger()
    : CurrentLevel(LogLevel::INFO),
      MaxFileSize(1024 * 1024),
      MaxFiles(5)
{
}

Logger& Logger::Instance()
{
    static Logger instance;
    return instance;
}

LogLevel Logger::GetCurrentLevel() const
{
    return CurrentLevel;
}

void Logger::LoadConfiguration(const std::string& configPath)
{
    std::map<std::string, std::string> config = ConfigReader::Parse(configPath);

    std::string level = config["Log.Level"];
    if (level == "INFO")
    {
        CurrentLevel = LogLevel::INFO;
    }
    else if (level == "WARNING")
    {
        CurrentLevel = LogLevel::WARNING;
    }
    else if (level == "ERROR")
    {
        CurrentLevel = LogLevel::ERROR;
    }
    else
    {
        CurrentLevel = LogLevel::FATAL;
    }

    Directory = config["Log.Directory"];
    BaseFileName = config["Log.FilName"];
    MaxFileSize = static_cast<size_t>(std::stoul(config["Log.MaxFileSize"]));
    MaxFiles = static_cast<size_t>(std::stoul(config["Log.MaxFiles"]));

    DiskFile::CreateDirectory(Directory);
    OpenLogFile();
}

void Logger::Log(LogLevel level, const char* format, ...)
{
    if (level < CurrentLevel)
    {
        return;
    }

    std::lock_guard<std::mutex> lock(Mutex);

    RotateIfNeeded();

    std::time_t now = std::time(nullptr);
    std::tm* localTime = (std::localtime)(&now);

    std::ostringstream header;
    header << std::put_time(localTime, "%Y-%m-%d %H:%M:%S")
           << " [TID:" << GetThreadIdString()
           << "] [" << LevelToString(level) << "] ";

    char messageBuffer[1024];
    va_list args;
    va_start(args, format);
    std::vsnprintf(messageBuffer, sizeof(messageBuffer), format, args);
    va_end(args);

    LogFile.WriteLine(header.str() + messageBuffer);
    LogFile.Flush();
}

void Logger::RotateIfNeeded()
{
    if (LogFile.GetSize() >= MaxFileSize)
    {
        LogFile.Close();
        RotateFiles();
        OpenLogFile();
    }
}

void Logger::OpenLogFile()
{
    std::string path = Directory + "/" + BaseFileName;
    LogFile.Open(path, true);
}

void Logger::RotateFiles()
{
    for (int i = static_cast<int>(MaxFiles) - 1; i > 0; --i)
    {
        std::string prev = Directory + "/" + BaseFileName + "." + std::to_string(i - 1);
        std::string next = Directory + "/" + BaseFileName + "." + std::to_string(i);
        DiskFile::Remove(next);
        std::rename(prev.c_str(), next.c_str());
    }

    std::string current = Directory + "/" + BaseFileName;
    std::string rotated = Directory + "/" + BaseFileName + ".0";
    DiskFile::Remove(rotated);
    std::rename(current.c_str(), rotated.c_str());
}

std::string Logger::LevelToString(LogLevel level)
{
    switch (level)
    {
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::WARNING:
            return "WARNING";
        case LogLevel::ERROR:
            return "ERROR";
        case LogLevel::FATAL:
            return "FATAL";
        default:
            return "UNKNOWN";
    }
}

std::string Logger::GetThreadIdString()
{
    std::ostringstream ss;
    ss << std::this_thread::get_id();
    return ss.str();
}
