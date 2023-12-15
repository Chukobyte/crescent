#pragma once

#include <filesystem>

namespace FileSystemHelper {
    struct ReturnStatus{
        std::optional<bool> returnValue;
        std::error_code errorCode;

        operator bool() const {
            return returnValue.value_or(!HasError());
        }

        [[nodiscard]] bool HasError() const {
            return errorCode.value() != 0;
        }
    };

inline std::string GetCurrentDir() {
    return std::filesystem::current_path().string();
}

void WriteFile(const std::string& filePath, const std::string& fileText);

inline ReturnStatus DoesDirectoryExist(const std::filesystem::path& filePath) {
    ReturnStatus status;
    status.returnValue = std::filesystem::is_directory(filePath, status.errorCode);
    return status;
}

inline ReturnStatus DoesFileExist(const std::filesystem::path& filePath) {
    ReturnStatus status;
    status.returnValue = std::filesystem::exists(filePath, status.errorCode);
    return status;
}


inline ReturnStatus IsDirectoryEmpty(const std::string& filePath) {
    ReturnStatus status;
    status.returnValue = std::filesystem::is_empty(filePath, status.errorCode);
    return status;
}

inline bool DirectoryExistsAndIsEmpty(const std::string& filePath) {
    return DoesDirectoryExist(filePath) && IsDirectoryEmpty(filePath);
}

inline ReturnStatus CreateDirectory(const std::string& filePath) {
    ReturnStatus status;
    status.returnValue = std::filesystem::create_directory(filePath, status.errorCode);
    return status;
}

ReturnStatus RecreateDirectory(const std::string& dirPath);

template<typename T>
inline ReturnStatus CopyFile(const T& source,
                     const T& target,
                     const std::filesystem::copy_options copyOptions = std::filesystem::copy_options::update_existing | std::filesystem::copy_options::recursive) {
    ReturnStatus status;
    std::filesystem::copy(source, target, copyOptions, status.errorCode);
    return status;
}

void CopyFilesRecursively(const std::filesystem::path& source, const std::filesystem::path& dest, const std::vector<std::string>& exclusionPatterns = {});

} // namespace FileSystemHelper
