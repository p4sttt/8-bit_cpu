#include "logger.hpp"

#include <bitset>
#include <iostream>
#include <sstream>

#include "cpu.hpp"
#include "options.hpp"

void Logger::Log(const std::string& msg, log_level_t log_level) {
    auto& options = CMDOptions::GetInstance();
    if (!options.GetIsLog())
        return;

    switch (log_level) {
    case INFO:
        std::cout << "[INFO] ";
        break;
    case ERROR:
        std::cout << "[ERROR] ";
        break;
    }
    std::cout << msg;
}

void Logger::LogInfo(const std::string& msg) { Log(msg, INFO); }

void Logger::LogError(const std::string& msg) { Log(msg, ERROR); }

void Logger::LogHex(opcode_t val) {
    std::stringstream ss;
    ss << std::hex << std::uppercase << val;
    LogInfo("0x" + ss.str());
}

void Logger::LogBin(opcode_t val) {
    std::bitset<INSTRUCTION_SIZE> bits(val);
    LogInfo("0b" + bits.to_string());
}