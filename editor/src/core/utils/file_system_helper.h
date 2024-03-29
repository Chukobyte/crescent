#pragma once

#include <vector>
#include <filesystem>
#include <optional>
#include <functional>

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

inline std::filesystem::path GetCurrentDir() {
    return std::filesystem::current_path();
}

inline std::string GetCurrentDirStr() {
    return std::filesystem::current_path().string();
}

inline ReturnStatus SetCurrentDir(const std::filesystem::path& directory) {
    ReturnStatus status;
    std::filesystem::current_path(directory, status.errorCode);
    return status;
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

inline ReturnStatus CreateDirectory(const std::filesystem::path& filePath) {
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

ReturnStatus MoveFile(const std::filesystem::path& source, const std::filesystem::path& dest);

ReturnStatus DeleteDirectory(const std::filesystem::path& directory);

ReturnStatus DeleteAllInDirectory(const std::filesystem::path& directory, const std::vector<std::filesystem::path>& exclusions = {});

void CopyFilesRecursively(const std::filesystem::path& source, const std::filesystem::path& dest, const std::vector<std::string>& exclusionPatterns = {});

void CopyFilesRecursivelyWithExtension(const std::filesystem::path& source, const std::filesystem::path& dest, const std::vector<std::string>& extensions);

ReturnStatus ForEachFile(const std::filesystem::path& directory, std::function<void(const std::filesystem::directory_entry&)> func);

void ZipDirectory(const std::string& zipName, const std::filesystem::path& sourceDirectory);
} // namespace FileSystemHelper
