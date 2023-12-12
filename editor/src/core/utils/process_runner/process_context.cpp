#include "process_context.h"

#include "../helper.h"

#if defined(WIN32) || defined(WIN64)

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
#include <vector>

bool ProcessContext::Start(const std::string& processPath, const std::string& startArgs) {
    if (IsRunning()) {
        return false;
    }
    pid = fork();
    if (pid < 0) {
        std::cerr << "Error creating fork!" << std::endl;
        return false;
    } else if (pid > 0) {
        // Parent
        int status;
        waitpid(pid, &status, WNOHANG);
    } else {
        // Child
        auto SplitStartArgsString = [](const std::string& path, const std::string& startArgText) -> std::vector<const char*> {
            static Helper::StringSplitter splitter;
            splitter.Clear();
            return splitter.Split(path + " " + startArgText).ToConst();
        };
        auto args = SplitStartArgsString(processPath, startArgs);
        execv(processPath.c_str(), const_cast<char* const*>(args.data()));
    }
    return true;
}

void ProcessContext::Stop() {
    if (!IsRunning()) {
        return;
    }
    kill(pid, SIGUSR1);
    pid = 0;
}

bool ProcessContext::IsRunning() const {
    if (pid != 0) {
        return kill(pid, 0) == 0;
    }
    return false;
}

#endif
