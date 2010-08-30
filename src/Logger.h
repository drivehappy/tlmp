#pragma once

#include <stdarg.h>
#include <string>
using namespace std;

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;

#include "Timer.h"

namespace TLMP
{
  static wstring& convertAsciiToWide(string str) {
    size_t origsize = strlen(str.c_str()) + 1;
    const size_t newsize = 1024;
    size_t convertedChars = 0;
    wchar_t wcstring[newsize];
    mbstowcs_s(&convertedChars, wcstring, origsize, str.c_str(), _TRUNCATE);

    wstring *newStr = new wstring();
    newStr->append(wcstring);

    return *newStr;
  };

  enum LoggingLevel {
    Verbose = 0, 
    Info, 
    Error
  };

  enum LoggingFilter {
    Character = 0,
    Layout,
    Equipment,
    ResourceManager,
  };

  class Logger
  {
  private:
    bool m_bTimeWritten;

    wofstream m_outFile;
    Timer m_logTimer;
    LoggingLevel m_logLevel;

    wstring wFormat(const wchar_t *format, ...);
    wstring wvFormat(const wchar_t *format, va_list args);

  public:
    Logger(const char* filename);
    ~Logger();

    void WriteLine(LoggingLevel level, const wchar_t *fmt, ...);
    void Write(LoggingLevel level, const wchar_t *fmt, ...);

    void SetLoggingLevel(LoggingLevel level);
    
  };

};
