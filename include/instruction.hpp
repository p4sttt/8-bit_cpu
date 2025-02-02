#pragma once

#include <functional>
#include <memory>
#include <unordered_map>

#include "cpu.hpp"

class Instruction {
  public:
    virtual void Execute(CPUState& state) = 0;
    virtual ~Instruction() = default;
};

class MoveInstruction : public Instruction {
  public:
    using endianness_t = enum {
        LOW = 0,
        HIGH = 1,
    };

  private:
    endianness_t mode_;
    opcode_t reg_idx_;
    opcode_t value_;

  public:
    MoveInstruction(endianness_t mode, opcode_t reg_idx, opcode_t value);

    void Execute(CPUState& state);
};

class LoadInstruction : public Instruction {
  private:
    opcode_t reg_idx_;

  public:
    LoadInstruction(opcode_t reg_idx);

    void Execute(CPUState& state) override;
};

class StoreInstruction : public Instruction {
  private:
    opcode_t reg_idx_;

  public:
    StoreInstruction(opcode_t reg_idx);

    void Execute(CPUState& state) override;
};

class BranchInstruction : public Instruction {
  public:
    using cond_t = enum {
        NONE = 0,
        EQ = 1,
        NE = 2,
        GE = 3,
    };

  private:
    cond_t cond_;
    opcode_t reg_idx_;

  public:
    BranchInstruction(cond_t cond, opcode_t reg_idx);

    void Execute(CPUState& state) override;
};

class ArithmeticalLogicalInstruction : public Instruction {
  public:
    using alu_t = enum {
        ADD = 0,
        SUB = 1,
        AND = 2,
        OR = 3,
    };

  private:
    mode_t mode_;
    opcode_t reg_idx_1_;
    opcode_t reg_idx_2_;

  public:
    ArithmeticalLogicalInstruction(mode_t mode, opcode_t reg_idx_1, opcode_t reg_idx_2);

    void Execute(CPUState& state) override;
};

class InstructionFactory {
  private:
    using InstructionCreator_ = std::function<std::unique_ptr<Instruction>(opcode_t)>;
    std::unordered_map<opcode_t, InstructionCreator_> instructions_;

  public:
    InstructionFactory();

    void RegisterInstruction(opcode_t opcode, InstructionCreator_ creator);
    std::unique_ptr<Instruction> Create(opcode_t opcode);
};

extern InstructionFactory g_instruction_factory;