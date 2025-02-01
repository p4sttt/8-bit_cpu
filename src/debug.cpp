#include "debug.hpp"

#include <bitset>
#include <iostream>
#include <memory>

#include "cpu.hpp"
#include "instruction.hpp"

// ЭТОТ КОД ПИСАЛСЯ В СОСТОЯНИИ ИЗМЕНОЕННОГО СОЗНАНИЯ
// МОГУТ ПОДТЕКАТЬ КРАСНЫЕ СЛЕЗЫ (возможно кровавые :))
DebugConsole::DebugConsole() {
    RegisterCommand(kHelpMessage, [this](CPUState& state, const std::string& input) {
        std::cout << "Commands:" << std::endl;
        std::cout << "\t" << kHelpMessage << " - print help message" << std::endl;
        std::cout << "\t" << kNextInstruction << " - next instruction" << std::endl;
        std::cout << "\t" << kPrintRegisters << " - print Rn register" << std::endl;
        std::cout << "\t" << kPrintCurrentInstruction << " - print current instruction"
                  << std::endl;
        std::cout << "\t" << kPrintMemory << " - print memory at addr" << std::endl;
        std::cout << "\t" << kQuit << " - quit" << std::endl;
    });

    RegisterCommand(kNextInstruction,
                    [this](CPUState& state, const std::string& input) { running_ = false; });

    RegisterCommand(kPrintRegisters, [this](CPUState& state, const std::string& input) {
        if (input.size() > 5) {
            opcode_t reg_idx = std::atoi(input.substr(5).c_str());
            std::cout << "R" << std::to_string(reg_idx) << "="
                      << std::to_string(state.GetRegister(reg_idx)) << "; ";
            return;
        }

        for (opcode_t i = 0; i < kRegisterCount; ++i) {
            std::cout << "R" << std::to_string(i + 1) << "=" << std::to_string(state.GetRegister(i))
                      << "; ";
        }
        std::cout << std::endl;
    });

    RegisterCommand(kPrintMemory, [this](CPUState& state, const std::string& input) {
        if (input.size() > 5) {
            opcode_t addr = std::atoi(input.substr(5).c_str());
            std::cout << std::to_string(addr) << ": "
                      << std::bitset<kBitness>(state.GetMemory(addr)) << std::endl;
            return;
        }

        for (opcode_t i = 0; i < kMemorySize; ++i) {
            std::cout << std::to_string(i) << ": " << std::bitset<kBitness>(state.GetMemory(i))
                      << std::endl;
        }
    });

    RegisterCommand(kPrintCurrentInstruction, [this](CPUState& state, const std::string& input) {
        std::cout << std::to_string(state.GetProgramCounter()) << ": "
                  << std::bitset<kBitness>(state.GetMemory(state.GetProgramCounter())) << std::endl;
    });

    RegisterCommand(kQuit, [this](CPUState& state, const std::string& input) {
        std::cout << "quit..." << std::endl;
        exit(EXIT_SUCCESS);
    });
}

void DebugConsole::RegisterCommand(const std::string& name, Handler_ handler) {
    commands_[name] = handler;
}

void DebugConsole::ProcessInput(CPUState& state) {
    while (running_) {
        std::string input;
        std::cout << "#> ";
        getline(std::cin, input);

        if (std::cin.eof()) {
            std::cout << "\nquit..." << std::endl;
            exit(EXIT_SUCCESS);
        }

        if (commands_.find(input) != commands_.end()) {
            commands_[input](state, input);
        }
    }
}

DebugWrapper::DebugWrapper(std::unique_ptr<Instruction> instr)
    : wrapped_instruction_(std::move(instr)) {}

void DebugWrapper::Execute(CPUState& state) {
    // TODO: не нравится, что при каждом исполнение нужно создавать объект
    // консоли
    DebugConsole console;
    console.ProcessInput(state);
    wrapped_instruction_->Execute(state);
}