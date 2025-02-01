#include "cpu.hpp"
#include "options.hpp"

int main(int argc, char** argv) {
    auto& options = CMDOptions::GetInstance();
    options.Parse(argc, argv);

    CPUState state;
    state.LoadProgram(options.GetFile());

    CPU cpu(state);
    cpu.RunProgram();
}