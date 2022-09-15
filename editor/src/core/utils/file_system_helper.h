#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace FileSystemHelper {
inline std::string GetCurrentDir() {
    return std::filesystem::current_path().string();
}

inline void WriteFile(const std::string& filePath, const std::string& fileText) {
    std::ofstream myFile(filePath);
    std::stringstream textStream;
    textStream.str(fileText);
    myFile << textStream.rdbuf();
    myFile.close();
}

inline bool DoesDirectoryExist(const std::string& filePath) {
    return std::filesystem::is_directory(filePath);
}

inline bool DoesFileExist(const std::string& filePath) {
    return std::filesystem::exists(filePath);
}


inline bool IsDirectoryEmpty(const std::string& filePath) {
    return std::filesystem::is_empty(filePath);
}

inline bool DirectoryExistsAndIsEmpty(const std::string& filePath) {
    return DoesDirectoryExist(filePath) && IsDirectoryEmpty(filePath);
}

inline bool CreateDirectory(const std::string& filePath) {
    return std::filesystem::create_directory(filePath);
}

template<typename T>
inline bool CopyFile(const T& source,
                     const T& target,
                     const std::filesystem::copy_options copyOptions = std::filesystem::copy_options::update_existing | std::filesystem::copy_options::recursive) {
    std::error_code ec;
    std::filesystem::copy(source, target, copyOptions, ec);
    return ec.value() == 0;
}
} // namespace FileSystemHelper
