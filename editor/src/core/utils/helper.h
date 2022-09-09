#pragma once

#include <string>

namespace Helper {
inline std::string BoolToString(bool value) {
    return value == true ? "true" : "false";
}

inline std::string BoolToStringCapital(bool value) {
    return value == true ? "True" : "False";
}

inline std::string RemoveExtensionFromFilePath(const std::string& filePath) {
    const size_t fileExtensionIndex = filePath.find_last_of('.');
    std::string newFilePath = filePath.substr(0, fileExtensionIndex);
    return newFilePath;
}

inline std::string ConvertFilePathToFilePathExtension(const std::string& filePath, const std::string& extension) {
    return RemoveExtensionFromFilePath(filePath) + extension;
}

inline std::string ConvertFilePathToFileName(const std::string& filePath, char delimiter = '/') {
    const size_t lastBackslashIndex = filePath.find_last_of(delimiter);
    std::string fileName = filePath.substr(0, lastBackslashIndex + 1);
    return fileName;
}

inline std::string ConvertFilePathToFileNameExtension(const std::string& filePath, const std::string& extension) {
    return ConvertFilePathToFilePathExtension(ConvertFilePathToFileName(filePath), extension);
}
} // namespace
