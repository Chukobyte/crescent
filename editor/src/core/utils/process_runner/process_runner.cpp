#include "process_runner.h"

bool ProcessRunner::Start(const std::string& processPath, const std::string& startArgs) {
    if (FileSystemHelper::DoesFileExist(processPath)) {
        return processContext.Start(processPath, startArgs);
    }
    return false;
}

void ProcessRunner::Stop() {
    processContext.Stop();
}

bool ProcessRunner::IsRunning() const {
    return processContext.IsRunning();
}
