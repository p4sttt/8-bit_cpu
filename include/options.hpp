#pragma once

#include <cstdbool>
#include <string>

class CMDOptions {
  private:
    std::string file_;
    bool debug_;
    bool log_;

    CMDOptions();
    void PrintHelp();

  public:
    CMDOptions(const CMDOptions& options) = delete;
    CMDOptions& operator=(const CMDOptions& options) = delete;

    static CMDOptions& GetInstance();
    void Parse(int argc, char** argv);

    const std::string& GetFile() const;
    bool GetIsDebug() const;
    bool GetIsLog() const;
};