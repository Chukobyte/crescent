#pragma once

#include <string>

#ifdef _WIN32
#include <process.h>
#include <windows.h>
#include <fstream>
#else
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <vector>
#include <cstring>
#include <algorithm>
#endif

class ProcessContext {
  public:
    bool Start(const std::string& processPath, const std::string& startArgs = "");
    void Stop();
    bool IsRunning() const;

  private:
#ifdef _WIN32
    STARTUPINFO startUpInfo;
    PROCESS_INFORMATION processInformation;
    FILE* stdOutStream = nullptr;
    std::streamoff stdOutInputStreamOffset = 0;
    std::ifstream stdOutInputStream = nullptr;
#else
    pid_t pid;
#endif
};
