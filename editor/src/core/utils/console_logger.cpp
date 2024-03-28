#include "console_logger.h"

// TODO: Capture log from running runtime executable

//--- Console Log Capture ---//
ConsoleLogCapture::ConsoleLogCapture(std::function<void(const char *, size_t)> callback) {
//    captureStdout = new std::capture::CaptureStdout(callback);
//    captureStderr = new std::capture::CaptureStderr(callback);
}

ConsoleLogCapture::~ConsoleLogCapture() {
//    delete captureStdout;
//    delete captureStderr;
}

//--- Console Logger ---//
std::shared_ptr<ConsoleLogCapture> ConsoleLogger::CaptureOutput() {
    return std::make_shared<ConsoleLogCapture>([this](const char *buff, size_t size) {
        CaptureCallback(buff, size);
    });
}

void ConsoleLogger::CaptureCallback(const char* buff, size_t size) {
    buffer += std::string(buff, size);
}

void ConsoleLogger::Clear() {
    buffer.clear();
}

void ConsoleLogger::AddEntry(const std::string &text) {
    buffer += text;
    buffer += "\n";
}

std::string& ConsoleLogger::GetText() {
    return buffer;
}
