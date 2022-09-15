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

bool ProcessContext::Start(const std::string& processPath, const std::string& startArgs) {
    if (IsRunning()) {
        return false;
    }
    pid = fork();
    if (pid < 0) {
        std::cerr << "Error creating fork!" << std::endl;
        return false;
    } else if (pid > 0) {
        auto SplitStartArgsString = [](const std::string& path, const std::string& startArgText) {
            std::vector<const char*> startArgsVec;
            startArgsVec.push_back(path.c_str());
            if (!startArgText.empty()) {
                static std::string temp;
                for(char i : startArgText) {
                    if(i == ' ') {
                        startArgsVec.push_back(temp.c_str());
                        temp.clear();
                    } else {
                        temp.push_back(i);
                    }
                }
                startArgsVec.push_back(temp.c_str());
                startArgsVec.push_back(nullptr);
                temp.clear();
            }
            return startArgsVec;
        };
        auto args = SplitStartArgsString(processPath, startArgs);
        execvp(processPath.c_str(), const_cast<char* const*>(args.data()));
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
    if (pid != 0) {
        return kill(pid, 0) == 0;
    }
    return false;
}

#endif
