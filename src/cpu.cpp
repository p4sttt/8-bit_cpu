#include "cpu.hpp"

#include <cstdint>
#include <fstream>
#include <ios>
#include <iostream>
#include <memory>

#include "debug.hpp"
#include "instruction.hpp"
#include "options.hpp"

CPUState::CPUState()
    : memory_()
    , regs_()
    , pc_(0)
    , sf_(false)
    , zf_(false) {}

opcode_t CPUState::GetMemory(opcode_t addr) const { return memory_[addr]; }
opcode_t CPUState::GetRegister(opcode_t idx) const {
    if (idx <= REGISTERS_COUNT) return regs_[idx];
    return 0;
}
opcode_t CPUState::GetProgramCounter() const { return pc_; }
bool CPUState::GetSignFlag() const { return sf_; }
bool CPUState::GetZeroFlag() const { return zf_; }
void CPUState::LoadProgram(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    file.read(reinterpret_cast<char*>(memory_.data()), MEMORY_SIZE);
    file.close();
}

void CPUState::SetMemory(opcode_t addr, opcode_t val) { memory_[addr] = val; }
void CPUState::SetRegister(opcode_t idx, opcode_t val) { regs_[idx] = val; }
void CPUState::InreaseProgramCounter() { pc_++; }
void CPUState::SetProgramCounter(opcode_t addr) { pc_ = addr; }
void CPUState::SetSignFlag(bool val) { sf_ = val; }
void CPUState::SetZeroFlag(bool val) { zf_ = val; }

CPU::CPU(CPUState& state)
    : state_(state) {}

void CPU::RunProgram() {
    while (state_.GetProgramCounter() < MEMORY_SIZE) {
        opcode_t opcode = state_.GetMemory(state_.GetProgramCounter());
        auto instr = g_instruction_factory.Create(opcode);
        instr->Execute(state_);
        state_.InreaseProgramCounter();
    }
}