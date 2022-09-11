#include "process_context.h"

#ifdef _WIN32

#define MAX_START_PROCESS_ARGS_BUFFER_SIZE 256

bool ProcessContext::Start(const std::string& processPath, const std::string& startArgs) {
    if (IsRunning()) {
        return false;
    }
    ZeroMemory(&startUpInfo, sizeof(startUpInfo));
    startUpInfo.cb = sizeof(startUpInfo);
    ZeroMemory(&processInformation, sizeof(processInformation));

    const std::string fullStartProcessArgs = processPath + " " + startArgs;

    TCHAR startProcessArgs[MAX_START_PROCESS_ARGS_BUFFER_SIZE];
    strcpy(startProcessArgs, fullStartProcessArgs.c_str());

    CreateProcess(processPath.c_str(),
                  startArgs.empty() ? nullptr : startProcessArgs,
                  nullptr,
                  nullptr,
                  FALSE,
                  0,
                  nullptr,
                  nullptr,
                  &startUpInfo,
                  &processInformation
                 );
    return true;
}

void ProcessContext::Stop() {
    if (!IsRunning()) {
        return;
    }
    TerminateProcess(processInformation.hProcess, 0);
    CloseHandle(processInformation.hProcess);
    CloseHandle(processInformation.hThread);
    ZeroMemory(&startUpInfo, sizeof(startUpInfo));
    startUpInfo.cb = sizeof(startUpInfo);
    ZeroMemory(&processInformation, sizeof(processInformation));
}

[[nodiscard]] bool ProcessContext::IsRunning() const {
    if (processInformation.hProcess != nullptr && processInformation.hThread != nullptr) {
        DWORD nCode = 0;
        const int result = GetExitCodeProcess(processInformation.hProcess, &nCode);
        if (result == 0) {
            // std::cerr << "Error in getting running process!\n" << nCode << std::endl;
        } else if (nCode != STILL_ACTIVE) {
            // std::cout << "Process terminated from client" << std::endl;
        } else {
            return true;
        }
    }
    return false;
}

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

namespace {
std::vector<std::string> SplitString(const std::string& s) {
    if (s.empty()) {
        return {};
    }
    std::vector<std::string> splitStrings;
    std::string temp = "";
    for(char i : s) {
        if(i==' ') {
            splitStrings.push_back(temp);
            temp = "";
        } else {
            temp.push_back(i);
        }
    }
    splitStrings.push_back(temp);
    return splitStrings;
}

std::vector<char*> GetFullArgs(const std::string& processPath, const std::string& startArgs) {
    std::vector<char*> args;
    const std::string fullStartArgs = startArgs.empty() ? processPath : processPath + " " + startArgs;
    std::vector<std::string> splitStringArgs = SplitString(fullStartArgs);
    std::transform(splitStringArgs.begin(), splitStringArgs.end(), std::back_inserter(args),
    [](std::string &s) {
        s.push_back(0);
        return &s[0];
    });
    args.push_back(nullptr);
    return args;
}
} // namespace

bool ProcessContext::Start(const std::string& processPath, const std::string& startArgs) {
    if (IsRunning()) {
        return false;
    }
    pid = fork();
    if (pid < 0) {
        std::cerr << "Error creating fork!" << std::endl;
        return false;
    } else if(pid > 0) {
//        std::cout << "In parent process!" << std::endl;
    } else {
        std::vector<char*> args = GetFullArgs(processPath, startArgs);
        execv(processPath.c_str(), args.data());
    }
    return true;
}

void ProcessContext::Stop() {
    if (!IsRunning()) {
        return;
    }
    kill(pid, SIGUSR1);
}

bool ProcessContext::IsRunning() const {
    while(waitpid(-1, 0, WNOHANG) > 0) {
        // Wait for defunct....
    }
    if (kill(pid, 0) == 0) {
        return true;
    }
    return false;
}

#endif
