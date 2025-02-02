#include "utils.hpp"

#include <bitset>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "cpu.hpp"
#include "options.hpp"

void utils::Log(const std::string& msg, log_level_t log_level) {
    auto& options = CMDOptions::GetInstance();
    if (!options.GetIsLog()) return;

    // switch (log_level) {
    // case INFO:
    //     std::cout << "[INFO] ";
    //     break;
    // case ERROR:
    //     std::cout << "[ERROR] ";
    //     break;
    // }    
    std::cout << msg;
}

void utils::LogInfo(const std::string& msg) { Log(msg, INFO); }

void utils::LogError(const std::string& msg) { Log(msg, ERROR); }

std::string utils::ToHex(opcode_t val) {
    std::stringstream ss;
    ss << "0x" << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
       << static_cast<int>(val);
    return ss.str();
}

std::string utils::ToBin(opcode_t val) {
    std::bitset<kBitness> bits(val);
    return "0b" + bits.to_string();
}

std::string utils::ToDec(opcode_t val) {
    std::stringstream ss;
    ss << std::dec << static_cast<int>(val);
    return ss.str();
}

opcode_t utils::ExtractBits(opcode_t val, opcode_t start, opcode_t n) {
    return static_cast<opcode_t>((val >> (kBitness - start - n)) & ((1 << n) - 1));
}