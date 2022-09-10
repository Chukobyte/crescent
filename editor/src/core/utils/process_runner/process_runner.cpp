#include "process_runner.h"

bool ProcessRunner::Start(const std::string& processPath, const std::string& startArgs) {
    if (FileSystemHelper::DoesFileExist(processPath)) {
        processContext.Start(processPath, startArgs);
        return true;
    }
    return false;
}

void ProcessRunner::Stop() {
    processContext.Stop();
}

bool ProcessRunner::IsRunning() const {
    return processContext.IsRunning();
}
