#pragma once

#include "cpu.hpp"

class Logger {
  private:
    using log_level_t = enum {
        INFO,
        ERROR,
    };

  public:
    static void Log(const std::string& msg, log_level_t log_level);
    static void LogInfo(const std::string& msg);
    static void LogError(const std::string& msg);
    static void LogHex(opcode_t val);
    static void LogBin(opcode_t val);
};