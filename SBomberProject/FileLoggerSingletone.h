#pragma once

#include <string>

//Template Singleton
class FileLoggerSingletone
{
public:
    static FileLoggerSingletone& getInstance()
    {
        static FileLoggerSingletone instance;
        return instance;
    }

    void __fastcall OpenLogFile(const std::string& FN);

    void CloseLogFile();

    void __fastcall WriteToLog(const std::string& str);

    void __fastcall WriteToLog(const std::string& str, int n);

    void __fastcall WriteToLog(const std::string& str, double d);

private:
    FileLoggerSingletone() {};

    FileLoggerSingletone(const FileLoggerSingletone& root) = delete;
    FileLoggerSingletone& operator=(const FileLoggerSingletone&) = delete;
};

//Template Proxy
class LoggerSingletone
{
public:
    static LoggerSingletone& getInstance()
    {
        static LoggerSingletone instance;
        return instance;
    }

    void __fastcall OpenLogFile(const std::string& FN)
    {
        FileLoggerSingletone::getInstance().OpenLogFile(FN);
    }

    void CloseLogFile()
    {
        FileLoggerSingletone::getInstance().CloseLogFile();
    }

    void __fastcall WriteToLog(const std::string& str)
    {
        FileLoggerSingletone::getInstance().WriteToLog(std::to_string(++_counter) + " " + str);
    }

    void __fastcall WriteToLog(const std::string& str, int n)
    {
        FileLoggerSingletone::getInstance().WriteToLog(std::to_string(++_counter) + " " + str, n);
    }

    void __fastcall WriteToLog(const std::string& str, double d)
    {
        FileLoggerSingletone::getInstance().WriteToLog(std::to_string(++_counter) + " " + str, d);
    }

private:
    LoggerSingletone()
    {
        _counter = 0;
    };

    LoggerSingletone(const LoggerSingletone& root) = delete;
    LoggerSingletone& operator=(const LoggerSingletone&) = delete;

    int _counter;
};
