#include "instruction.hpp"

#include <bitset>
#include <iostream>
#include <memory>

#include "cpu.hpp"
#include "debug.hpp"
#include "options.hpp"

InstructionFactory g_instruction_factory;

MoveInstruction::MoveInstruction(endianness_t mode,
                                 opcode_t reg_idx,
                                 opcode_t value)
    : mode_(mode)
    , reg_idx_(reg_idx)
    , value_(value) {}

void MoveInstruction::Execute(CPUState& state) {
    opcode_t prev = state.GetRegister(reg_idx_);
    switch (mode_) {
    case LOW:
        state.SetRegister(reg_idx_, EXTRACT_BITS(prev, 0, 4) | value_);
        break;
    case HIGH:
        state.SetRegister(
            reg_idx_, EXTRACT_BITS(prev, 4, 4) | ((value_ << 4) & 0b11110000));
        break;
    }
}

LoadInstruction::LoadInstruction(opcode_t reg_idx)
    : reg_idx_(reg_idx) {}

void LoadInstruction::Execute(CPUState& state) {
    opcode_t addr = state.GetRegister(reg_idx_);
    opcode_t val = state.GetMemory(addr);
    state.SetRegister(val, 0);
}

StoreInstruction::StoreInstruction(opcode_t reg_idx)
    : reg_idx_(reg_idx) {}

void StoreInstruction::Execute(CPUState& state) {
    opcode_t addr = state.GetRegister(0);
    opcode_t val = state.GetRegister(reg_idx_);
    state.SetMemory(addr, val);
}

ArithmeticalLogicalInstruction::ArithmeticalLogicalInstruction(
    mode_t mode, opcode_t reg_idx_1, opcode_t reg_idx_2)
    : mode_(mode)
    , reg_idx_1_(reg_idx_1)
    , reg_idx_2_(reg_idx_2) {}

void ArithmeticalLogicalInstruction::Execute(CPUState& state) {
    opcode_t operand_1 = state.GetRegister(reg_idx_1_);
    opcode_t operand_2 = state.GetRegister(reg_idx_1_);
    opcode_t result;

    switch (mode_) {
    case ADD:
        result = operand_1 + operand_2;
        break;
    case SUB:
        result = operand_1 - operand_2;
        break;
    case AND:
        result = operand_1 & operand_2;
        break;
    case OR:
        result = operand_1 | operand_2;
        break;
    }

    state.SetZeroFlag(result == 0);
    state.SetSignFlag(result < 0);

    state.SetRegister(reg_idx_2_, result);
}

InstructionFactory::InstructionFactory() {

    // MOV.L
    RegisterInstruction(0b00, [](opcode_t opcode) {
        using mode_t = MoveInstruction::endianness_t;
        mode_t mode = static_cast<mode_t>(EXTRACT_BITS(opcode, 0, 2));
        opcode_t reg_idx = EXTRACT_BITS(opcode, 2, 2);
        opcode_t value = EXTRACT_BITS(opcode, 4, 4);

        return std::make_unique<MoveInstruction>(mode, reg_idx, value);
    });

    // MOV.H
    RegisterInstruction(0b01, [](opcode_t opcode) {
        using mode_t = MoveInstruction::endianness_t;
        mode_t mode = static_cast<mode_t>(EXTRACT_BITS(opcode, 0, 2));
        opcode_t reg_idx = EXTRACT_BITS(opcode, 2, 2);
        opcode_t value = EXTRACT_BITS(opcode, 4, 4);

        return std::make_unique<MoveInstruction>(mode, reg_idx, value);
    });

    // LOAD
    RegisterInstruction(0b1010, [](opcode_t opcode) {
        opcode_t reg_idx = EXTRACT_BITS(opcode, 6, 2);

        return std::make_unique<LoadInstruction>(reg_idx);
    });

    // STR
    RegisterInstruction(0b1000, [](opcode_t opcode) {
        opcode_t reg_idx = EXTRACT_BITS(opcode, 6, 2);

        return std::make_unique<StoreInstruction>(reg_idx);
    });

    // ALU
    RegisterInstruction(0b11, [](opcode_t opcode) {
        using mode_t = ArithmeticalLogicalInstruction::alu_t;

        mode_t mode = static_cast<mode_t>(EXTRACT_BITS(opcode, 2, 2));
        opcode_t reg_idx_1 = EXTRACT_BITS(opcode, 4, 2);
        opcode_t reg_idx_2 = EXTRACT_BITS(opcode, 6, 2);
        return std::make_unique<ArithmeticalLogicalInstruction>(
            mode, reg_idx_1, reg_idx_2);
    });
}

void InstructionFactory::RegisterInstruction(opcode_t prefix,
                                             InstructionCreator_ creator) {
    instructions_[prefix] = creator;
}

std::unique_ptr<Instruction> InstructionFactory::Create(opcode_t opcode) {

    opcode_t prefix = EXTRACT_BITS(opcode, 0, 2);
    auto it = instructions_.find(prefix);
    if (it != instructions_.end()) {
        if (CMDOptions::GetInstance().GetIsDebug())
            return std::make_unique<DebugWrapper>(it->second(opcode));
        else
            return it->second(opcode);
    }

    // костыль для инструкций с префиксом 10
    prefix = EXTRACT_BITS(opcode, 0, 4);
    it = instructions_.find(prefix);
    if (it != instructions_.end()) {
        if (CMDOptions::GetInstance().GetIsDebug())
            return std::make_unique<DebugWrapper>(it->second(opcode));
        else
            return it->second(opcode);
    }

    return nullptr;
}