#pragma once

#include "singleton.h"

#include "stdcapture/stdcapture.h"

class ConsoleLogger : public Singleton<ConsoleLogger> {
  public:
    explicit ConsoleLogger(singleton) {}

    void StartCapture();
    void StopCapture();
    void Clear();
    std::string& GetText();


  private:
    void CaptureCallback(const char* buff, size_t size);

    std::string buffer;
    std::capture::CaptureStdout* captureStdout = nullptr;
    std::capture::CaptureStderr* captureStderr = nullptr;
};
