#pragma once

#include <string>

namespace Helper {
inline std::string BoolToString(bool value) {
    return value == true ? "true" : "false";
}

inline std::string BoolToStringCapital(bool value) {
    return value == true ? "True" : "False";
}

inline std::string ConvertFilePathToFilePathExtension(const std::string& filePath, const std::string& extension) {
    const size_t fileExtensionIndex = filePath.find_last_of('.');
    const std::string validFilePath = filePath.substr(0, fileExtensionIndex) + extension;
    return validFilePath;
}

inline std::string ConvertFilePathToFileName(const std::string& filePath, char delimiter = '/') {
    const size_t lastBackslashIndex = filePath.find_last_of(delimiter);
    const std::string fileName = filePath.substr(0, lastBackslashIndex + 1);
    return fileName;
}

inline std::string ConvertFilePathToFileNameExtension(const std::string& filePath, const std::string& extension) {
    return ConvertFilePathToFilePathExtension(ConvertFilePathToFileName(filePath), extension);
}
} // namespace
