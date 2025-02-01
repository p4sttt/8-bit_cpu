#include "options.hpp"

#include <getopt.h>
#include <iostream>

CMDOptions::CMDOptions()
    : file_()
    , debug_(false)
    , log_(false) {}

CMDOptions& CMDOptions::GetInstance() {
    static CMDOptions instance;
    return instance;
}

void CMDOptions::PrintHelp() {
    std::cout << "Usage: cpu [options] -f [filename]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "\t-h - print help message" << std::endl;
    std::cout << "\t-d - enable debug mode" << std::endl;
    std::cout << "\t-l - enable logging" << std::endl;
    std::cout << std::endl;
}

void CMDOptions::Parse(int argc, char** argv) {
    if (argc < 2) {
        PrintHelp();
        exit(EXIT_FAILURE);
    }

    int opt;
    optind = 1;
    while ((opt = getopt(argc, argv, "f:dlh")) != -1) {
        switch (opt) {
        case 'h':
            PrintHelp();
            exit(EXIT_SUCCESS);
            break;
        case 'f':
            file_ = optarg;
            break;
        case 'd':
            debug_ = true;
            break;
        case 'l':
            log_ = true;
            break;
        default:
            PrintHelp();
            exit(EXIT_FAILURE);
            break;
        }
    }
}

const std::string& CMDOptions::GetFile() const { return file_; }

bool CMDOptions::GetIsDebug() const { return debug_; }

bool CMDOptions::GetIsLog() const { return log_; }