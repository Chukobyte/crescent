#include "process_runner.h"
#include "../console_logger.h"

static std::shared_ptr<ConsoleLogCapture> logCapture;

bool ProcessRunner::Start(const std::string& processPath, const std::string& startArgs) {
    logCapture = ConsoleLogger::Get()->CaptureOutput();
    if (FileSystemHelper::DoesFileExist(processPath)) {
        return processContext.Start(processPath, startArgs);
    }
    return false;
}

void ProcessRunner::Stop() {
    processContext.Stop();
    logCapture.reset();
}

bool ProcessRunner::IsRunning() const {
    return processContext.IsRunning();
}
