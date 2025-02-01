#pragma once

#include <array>
#include <cstdbool>
#include <cstdint>
#include <string>

#define EXTRACT_BITS(opcode, n, m) (((opcode) >> (n)) & ((1 << (m)) - 1))

static constexpr int kBitness = 8;
static constexpr int kRegisterCount = 4;
static constexpr int kMemorySize = 10;

using opcode_t = uint8_t;

class CPUState {
  private:
    std::array<opcode_t, kMemorySize> memory_;
    std::array<opcode_t, kRegisterCount> regs_;
    opcode_t pc_;
    bool sf_, zf_;

  public:
    CPUState();

    void LoadProgram(const std::string& filename);

    opcode_t GetMemory(opcode_t addr) const;
    opcode_t GetRegister(opcode_t idx) const;
    opcode_t GetProgramCounter() const;
    bool GetSignFlag() const;
    bool GetZeroFlag() const;

    void SetMemory(opcode_t addr, opcode_t val);
    void SetRegister(opcode_t idx, opcode_t val);
    void InreaseProgramCounter();
    void SetProgramCounter(opcode_t addr);
    void SetSignFlag(bool val);
    void SetZeroFlag(bool val);
};

class CPU {
  private:
    CPUState& state_;

  public:
    CPU(CPUState& state);

    void RunProgram();
};
