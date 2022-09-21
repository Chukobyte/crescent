#pragma once

#include <string>

#include "../file_system_helper.h"

#include "process_context.h"

class ProcessRunner {
  public:
    bool Start(const std::string& processPath, const std::string& startArgs = "");
    void Stop();
    [[nodiscard]] bool IsRunning() const;

  private:
    ProcessContext processContext;
};
