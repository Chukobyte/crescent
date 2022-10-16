#include "console_logger.h"

void ConsoleLogger::StartCapture() {
    if (captureStdout == nullptr) {
        captureStdout = new std::capture::CaptureStdout([this](const char *buff, size_t size) {
            CaptureCallback(buff, size);
        });
    }
}

void ConsoleLogger::StopCapture() {
    if (captureStdout) {
        delete captureStdout;
        captureStdout = nullptr;
    }
}

void ConsoleLogger::CaptureCallback(const char* buff, size_t size) {
    buffer += std::string(buff, size);
}

void ConsoleLogger::Clear() {
    buffer.clear();
}

std::string& ConsoleLogger::GetText() {
    return buffer;
}
