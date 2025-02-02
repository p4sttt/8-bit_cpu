#pragma once

#include <cstdint>
#include <string>

static constexpr int kBitness = 8;
static constexpr int kRegisterCount = 4;
static constexpr int kMemorySize = 10;

using opcode_t = uint8_t;

namespace utils {

using log_level_t = enum {
    INFO,
    ERROR,
};

void Log(const std::string& msg, log_level_t log_level);
void LogInfo(const std::string& msg);
void LogError(const std::string& msg);

std::string ToHex(opcode_t val);
std::string ToBin(opcode_t val);
std::string ToDec(opcode_t val);
opcode_t ExtractBits(opcode_t val, opcode_t start, opcode_t n);

} // namespace utils
