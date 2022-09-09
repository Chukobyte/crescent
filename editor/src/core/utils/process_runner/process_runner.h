#pragma once

#include <string>

#include "../file_system_helper.h"

#include "process_context.h"

class ProcessRunner {
  public:
    bool StartProccess(const std::string &processPath, const std::string &startArgs = "") {
        if (FileSystemHelper::DoesFileExist(processPath)) {
            processContext.Start(processPath, startArgs);
            return true;
        }
        return false;
    }

    void StopProcess() {
        processContext.Stop();
    }

    bool IsProcessRunning() const {
        return processContext.IsRunning();
    }

  private:
    ProcessContext processContext;
};
