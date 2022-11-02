#pragma once

#include <string>
#include <vector>

namespace Helper {
// String manipulation
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

struct StringSplitter {
    StringSplitter() = default;

    StringSplitter(const std::string& text, char delimiter = ' ') {
        Split(text, delimiter);
    }

    StringSplitter& Split(const std::string& text, char delimiter = ' ') {
        std::string temp;
        for(char i : text) {
            if(i == delimiter) {
                splitUpStrings.emplace_back(temp);
                temp.clear();
            } else {
                temp.push_back(i);
            }
        }
        if (!temp.empty()) {
            splitUpStrings.emplace_back(temp);
        }
        return *this;
    }

    std::vector<const char*> ToConst() {
        std::vector<const char*> startArgsVec;
        for (auto& text : splitUpStrings) {
            startArgsVec.push_back(text.c_str());
        }
        startArgsVec.push_back(nullptr);
        return startArgsVec;
    }

    void Clear() {
        splitUpStrings.clear();
    }

    std::vector<std::string> splitUpStrings;
};

// Math
// TODO: Move into a math header
template<typename T>
inline T Min(T a, T b) {
    return a < b ? a : b;
}

template<typename T>
inline T Max(T a, T b) {
    return a > b ? a : b;
}

template<typename T>
inline T SafeDiv(T a, T b) {
    return b != 0.0f ? a / b : 0.0f;
}
} // namespace
