#include "instruction.hpp"

#include <bitset>
#include <iostream>
#include <memory>

#include "cpu.hpp"
#include "debug.hpp"
#include "options.hpp"

InstructionFactory g_instruction_factory;

MoveInstruction::MoveInstruction(endianness_t mode, opcode_t reg_idx, opcode_t value)
    : mode_(mode)
    , reg_idx_(reg_idx)
    , value_(value) {}

void MoveInstruction::Execute(CPUState& state) {
    opcode_t prev = state.GetRegister(reg_idx_);

    switch (mode_) {
    case LOW:
        utils::LogInfo("MOV.L");
        state.SetRegister(reg_idx_, utils::ExtractBits(prev, 0, 4) | value_);
        break;
    case HIGH:
        utils::LogInfo("MOV.H");
        state.SetRegister(reg_idx_, utils::ExtractBits(prev, 4, 4) | ((value_ << 4) & 0b11110000));
        break;
    }

    utils::LogInfo(" %R" + utils::ToDec(reg_idx_));
    utils::LogInfo(" $" + utils::ToBin(value_) + '\n');
}

LoadInstruction::LoadInstruction(opcode_t reg_idx)
    : reg_idx_(reg_idx) {}

void LoadInstruction::Execute(CPUState& state) {
    opcode_t addr = state.GetRegister(reg_idx_);
    opcode_t val = state.GetMemory(addr);

    utils::LogInfo("LDR");
    utils::LogInfo(" [R" + utils::ToDec(reg_idx_) + "] %R0\b");

    state.SetRegister(val, 0);
}

StoreInstruction::StoreInstruction(opcode_t reg_idx)
    : reg_idx_(reg_idx) {}

void StoreInstruction::Execute(CPUState& state) {
    opcode_t addr = state.GetRegister(0);
    opcode_t val = state.GetRegister(reg_idx_);

    utils::LogInfo("STR");
    utils::LogInfo("%R" + utils::ToDec(reg_idx_) + " [R0]");

    state.SetMemory(addr, val);
}

BranchInstruction::BranchInstruction(cond_t cond, opcode_t reg_idx)
    : cond_(cond)
    , reg_idx_(reg_idx) {}

void BranchInstruction::Execute(CPUState& state) {
    bool cond = false;
    opcode_t addr = state.GetRegister(reg_idx_);

    utils::LogInfo("[BranchInstruction::Execute]");

    switch (cond_) {
    case NONE:
        utils::LogInfo("B");
        cond = true;
    case EQ:
        utils::LogInfo("BEQ");
        cond = state.GetZeroFlag();
        break;
    case NE:
        utils::LogInfo("BNE");
        cond = !state.GetZeroFlag();
        break;
    case GE:
        utils::LogInfo("BGE");
        cond = !state.GetSignFlag();
        break;
    }

    utils::LogInfo("[R" + utils::ToDec(reg_idx_) + "]\n");

    if (cond) {
        state.SetProgramCounter(addr);
    }
}

ArithmeticalLogicalInstruction::ArithmeticalLogicalInstruction(mode_t mode,
                                                               opcode_t reg_idx_1,
                                                               opcode_t reg_idx_2)
    : mode_(mode)
    , reg_idx_1_(reg_idx_1)
    , reg_idx_2_(reg_idx_2) {}

void ArithmeticalLogicalInstruction::Execute(CPUState& state) {
    opcode_t operand_1 = state.GetRegister(reg_idx_1_);
    opcode_t operand_2 = state.GetRegister(reg_idx_1_);
    opcode_t result;

    switch (mode_) {
    case ADD:
        utils::LogInfo("ADD");
        result = operand_1 + operand_2;
        break;
    case SUB:
        utils::LogInfo("SUB");
        result = operand_1 - operand_2;
        break;
    case AND:
        utils::LogInfo("AND");
        result = operand_1 & operand_2;
        break;
    case OR:
        utils::LogInfo("OR");
        result = operand_1 | operand_2;
        break;
    }

    utils::LogInfo(" %R" + utils::ToDec(reg_idx_1_));
    utils::LogInfo(" %R" + utils::ToDec(reg_idx_2_) + '\n');

    state.SetZeroFlag(result == 0);
    state.SetSignFlag(utils::ExtractBits(result, 0, 1) == 1);

    state.SetRegister(reg_idx_2_, result);
}

InstructionFactory::InstructionFactory() {

    // MOV.L
    RegisterInstruction(0b00, [](opcode_t opcode) {
        using mode_t = MoveInstruction::endianness_t;
        mode_t mode = static_cast<mode_t>(utils::ExtractBits(opcode, 0, 2));
        opcode_t reg_idx = utils::ExtractBits(opcode, 2, 2);
        opcode_t value = utils::ExtractBits(opcode, 4, 4);

        return std::make_unique<MoveInstruction>(mode, reg_idx, value);
    });

    // MOV.H
    RegisterInstruction(0b01, [](opcode_t opcode) {
        using mode_t = MoveInstruction::endianness_t;
        mode_t mode = static_cast<mode_t>(utils::ExtractBits(opcode, 0, 2));
        opcode_t reg_idx = utils::ExtractBits(opcode, 2, 2);
        opcode_t value = utils::ExtractBits(opcode, 4, 4);

        return std::make_unique<MoveInstruction>(mode, reg_idx, value);
    });

    // LOAD
    RegisterInstruction(0b1000, [](opcode_t opcode) {
        opcode_t reg_idx = utils::ExtractBits(opcode, 6, 2);

        return std::make_unique<LoadInstruction>(reg_idx);
    });

    // STORE
    RegisterInstruction(0b1001, [](opcode_t opcode) {
        opcode_t reg_idx = utils::ExtractBits(opcode, 6, 2);

        return std::make_unique<StoreInstruction>(reg_idx);
    });

    // BRANCH
    RegisterInstruction(0b1010, [](opcode_t opcode) {
        using mode_t = BranchInstruction::cond_t;
        mode_t mode = static_cast<mode_t>(utils::ExtractBits(opcode, 4, 2));
        opcode_t reg_idx = utils::ExtractBits(opcode, 6, 2);

        return std::make_unique<BranchInstruction>(mode, reg_idx);
    });

    // ALU
    RegisterInstruction(0b11, [](opcode_t opcode) {
        using mode_t = ArithmeticalLogicalInstruction::alu_t;

        mode_t mode = static_cast<mode_t>(utils::ExtractBits(opcode, 2, 2));
        opcode_t reg_idx_1 = utils::ExtractBits(opcode, 4, 2);
        opcode_t reg_idx_2 = utils::ExtractBits(opcode, 6, 2);
        return std::make_unique<ArithmeticalLogicalInstruction>(mode, reg_idx_1, reg_idx_2);
    });
}

void InstructionFactory::RegisterInstruction(opcode_t prefix, InstructionCreator_ creator) {
    instructions_[prefix] = creator;
}

std::unique_ptr<Instruction> InstructionFactory::Create(opcode_t opcode) {
    auto create_instruction = [&](opcode_t prefix) -> std::unique_ptr<Instruction> {
        auto it = instructions_.find(prefix);
        if (it == instructions_.end()) return nullptr;

        auto instance = it->second(opcode);
        if (CMDOptions::GetInstance().GetIsDebug())
            return std::make_unique<DebugWrapper>(std::move(instance));

        return instance;
    };

    if (auto result = create_instruction(utils::ExtractBits(opcode, 0, 2))) return result;

    return create_instruction(utils::ExtractBits(opcode, 0, 4));
}