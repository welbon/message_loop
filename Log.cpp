#include "Stdafx.h"
#include "Log.h"
#include <time.h>
#include <vector>

std::string _File;
bool g_need_log = false;

std::string UnicodeToAnsi(const wchar_t* buf)
{
  int len = ::WideCharToMultiByte(CP_ACP, 0, buf, -1, NULL, 0, NULL, NULL);
  if (len == 0) 
    return "";

  std::vector<char> utf8(len);
  len = ::WideCharToMultiByte(CP_ACP, 0, buf, -1, &utf8[0], len, NULL, NULL); 
  if(len == 0)
    return "";

  return &utf8[0];
}

extern std::ostream& operator<<(std::ostream& out, const wchar_t* wstr) {
  std::wstring wstdstr = std::wstring(wstr);
  std::string converted_string = UnicodeToAnsi(wstr);
  return out << converted_string;
}

extern std::ostream& operator<<(std::ostream& out, const std::wstring& wstr) {
  return out << std::string(wstr.begin(), wstr.end());
}

void LogMessage::Initialize(const std::wstring file, 
                            const bool need_log) {
  std::string file_ascii(UnicodeToAnsi(file.c_str()));
  _File = file_ascii;
  g_need_log = need_log;
}

LogMessage::LogMessage(int fake_level, 
                       const std::string& file_name,
                       const int line)
  : fake_level_((LogLevel)fake_level)
  , line_(line)
  , file_(file_name) {

  file_ = file_.substr(file_.rfind("\\") + 1, file_.length() - 1);
}

std::string LogMessage::FormatInt64ToTimeString() {

  std::string time_string;
  SYSTEMTIME sys_time;
  ::GetLocalTime(&sys_time);
  char time_buffer[50] = { 0 };
  sprintf(time_buffer, "%d-%d-%d %02d:%02d:%02d %03d", 
    sys_time.wYear, 
    sys_time.wMonth, 
    sys_time.wDay, 
    sys_time.wHour, 
    sys_time.wMinute,
    sys_time.wSecond,
    sys_time.wMilliseconds);

  time_string = time_buffer;
  return time_string;
}

std::string LogMessage::GetLevel() {

  std::string level_text;
  switch (fake_level_) 
  {
  case INFO:
    level_text = "INFO";
    break;

  case WARNING:
    level_text = "WARNING";
    break;

  case ERR:
    level_text = "ERROR";
    break;
  }
  return level_text;
}

LogMessage::~LogMessage() {

#if _DEBUG
  g_need_log = true;
#endif //

  if (!g_need_log) {
    return ;
  }

  std::string log_text;
  std::ostringstream new_line_header;
  new_line_header << "["
    << GetLevel() << ": "
    << FormatInt64ToTimeString()
    << "/" << file_
    << "(" << line_ << ")"
    << "/~" << GetCurrentThreadId()
    << "]";

  log_text += new_line_header.str();
  log_text += " ";
  log_text += stream_.str();
  log_text += "\n";
  
  if (_File.empty()) {
    char app_file[MAX_PATH] =  { 0 };
    ::GetModuleFileNameA(NULL, app_file, MAX_PATH);
    _File = app_file;
    _File = _File.substr(0, _File.rfind("\\"));
    _File += "\\Debug.log";
  }
  OutputDebugStringA(log_text.c_str());

  FILE* f = fopen(_File.c_str(), "a+b");
  if (f) {
    fwrite(log_text.c_str(), sizeof(char), log_text.length(), f);
    fclose(f);
  }
}

