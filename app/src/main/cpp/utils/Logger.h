#pragma once
#include <android/log.h>

class Logger {
public:
  template<typename... Args>
  static void info(const char* fmt, Args... args) {
    __android_log_print(ANDROID_LOG_INFO, "MarkClient", fmt, args...);
  }
  
  static void info(const char* msg) {
    __android_log_print(ANDROID_LOG_INFO, "MarkClient", "%s", msg);
  }
};
