#pragma once

#include <string>
#include <iostream>
#include <fstream>

class FileLogger
{
public:
    FileLogger(std::string fileName = "log.txt");
    ~FileLogger();

    void __fastcall WriteToLog(const std::string& str);

    void __fastcall WriteToLog(const std::string& str, int n);

    void __fastcall WriteToLog(const std::string& str, double d);

private:
    
    std::ofstream _logOut;
};

