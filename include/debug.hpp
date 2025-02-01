#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "instruction.hpp"

class DebugConsole {
  private:
    using Handler_ = std::function<void(CPUState&, const std::string&)>;

    std::unordered_map<std::string, Handler_> commands_;
    bool running_ = true;

    static constexpr const char* kHelpMessage = "help";
    static constexpr const char* kNextInstruction = "next";
    static constexpr const char* kPrintRegisters = "regs";
    static constexpr const char* kPrintMemory = "memo";
    static constexpr const char* kQuit = "quit";
    static constexpr const char* kPrintCurrentInstruction = "instr";
    

  public:
    DebugConsole();
    void RegisterCommand(const std::string& name, Handler_ handler);

    void ProcessInput(CPUState& state);
};

class DebugWrapper : public Instruction {
  private:
    std::unique_ptr<Instruction> wrappedInstruction_;

  public:
    DebugWrapper(std::unique_ptr<Instruction> instr);

    void Execute(CPUState& state) override;
};