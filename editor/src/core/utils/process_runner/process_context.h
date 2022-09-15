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
    bool Start(const std::string& startArgText, const std::string& startArgs = "");
    void Stop();
    bool IsRunning() const;

  private:
#ifdef _WIN32
    STARTUPINFO startUpInfo;
    PROCESS_INFORMATION processInformation;
    FILE* stdOutStream = nullptr;
    std::streamoff stdOutInputStreamOffset = 0;
    std::ifstream stdOutInputStream;
#else
    pid_t pid = 0; // 0 assumes pid hasn't been assigned
#endif
};
