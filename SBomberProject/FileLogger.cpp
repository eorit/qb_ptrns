#include "FileLogger.h"
#include "MyTools.h"

FileLogger::FileLogger(std::string fileName)
{
    _logOut.open(fileName, std::ios_base::out);
}

FileLogger::~FileLogger()
{
    if (_logOut.is_open())
    {
        _logOut.close();
    }
}

void __fastcall FileLogger::WriteToLog(const std::string& str)
{
    if (_logOut.is_open())
    {
        _logOut << MyTools::GetCurDateTime() << " - " << str << std::endl;
    }
}

void __fastcall FileLogger::WriteToLog(const std::string& str, int n)
{
    if (_logOut.is_open())
    {
        _logOut << MyTools::GetCurDateTime() << " - " << str << n << std::endl;
    }
}

void __fastcall FileLogger::WriteToLog(const std::string& str, double d)
{
    if (_logOut.is_open())
    {
        _logOut << MyTools::GetCurDateTime() << " - " << str << d << std::endl;
    }
}
