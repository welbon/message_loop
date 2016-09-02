#ifndef UTIL_LOG_H_
#define UTIL_LOG_H_

#pragma once

#include <iostream>
#include <string>
#include <sstream>

enum LogLevel {
  INFO,
  WARNING,
  ERR,
};

/** A simple log message 
*
* @author: welbon
*
* @date 2012-08-14
*/
class LogMessage {
public:
  LogMessage(int fake_level, 
             const std::string& file_name,
             const int line);

  virtual ~LogMessage();

  static void Initialize(const std::wstring file, bool log);
  std::ostringstream& stream() { return stream_; }

protected:
  std::string FormatInt64ToTimeString();
  std::string GetLevel();

protected:
  LogLevel fake_level_;
  int line_;
  std::string file_;
  std::ostringstream stream_;
};

#define LOG(level) LogMessage(level, __FILE__, __LINE__).stream()

extern std::ostream& operator<<(std::ostream& out, const wchar_t* wstr);
extern std::ostream& operator<<(std::ostream& out, const std::wstring& wstr);

#endif //UTIL_LOG_H_