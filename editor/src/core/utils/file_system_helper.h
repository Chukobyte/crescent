#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace FileSystemHelper {
inline std::string GetCurrentDirectory() {
    return std::filesystem::current_path().string();
}

inline void WriteFile(const std::string &filePath, const std::string& fileText) {
    std::ofstream myFile(filePath);
    std::stringstream textStream;
    textStream.str(fileText);
    myFile << textStream.rdbuf();
}
}
