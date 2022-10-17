#pragma once

#include <memory>

#include "singleton.h"

#include "stdcapture/stdcapture.h"

class ConsoleLogCapture {
  public:
    explicit ConsoleLogCapture(std::function<void(const char*, size_t)> callback);
    ~ConsoleLogCapture();

  private:
    std::capture::CaptureStdout* captureStdout = nullptr;
    std::capture::CaptureStderr* captureStderr = nullptr;
};

class ConsoleLogger : public Singleton<ConsoleLogger> {
  public:
    explicit ConsoleLogger(singleton) {}

    std::shared_ptr<ConsoleLogCapture> CaptureOutput();
    void Clear();
    std::string& GetText();

  private:
    void CaptureCallback(const char* buff, size_t size);

    std::string buffer;
    std::shared_ptr<std::capture::CaptureStdout> captureStdout;
    std::shared_ptr<std::capture::CaptureStderr> captureStderr;
};
