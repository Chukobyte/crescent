#include "file_system_helper.h"

#include <iostream>
#include <fstream>
#include <sstream>

void FileSystemHelper::WriteFile(const std::string &filePath, const std::string &fileText) {
    std::ofstream myFile(filePath);
    std::stringstream textStream;
    textStream.str(fileText);
    myFile << textStream.rdbuf();
    myFile.close();
}

FileSystemHelper::ReturnStatus FileSystemHelper::RecreateDirectory(const std::string &dirPath) {
    // Delete if exists
    ReturnStatus status;
    if (std::filesystem::exists(dirPath)) {
        std::filesystem::remove_all(dirPath, status.errorCode);
    }
    // Now create the directory
    status.returnValue = std::filesystem::create_directory(dirPath, status.errorCode);
    return status;
}

void FileSystemHelper::CopyFilesRecursively(const std::filesystem::path &source, const std::filesystem::path &dest, const std::vector<std::string>& exclusionPatterns) {
    static auto MatchesExclusionPattern = [](const std::vector<std::string>& exclusionPatterns, const std::filesystem::directory_entry& entry) {
        for (const auto& pattern : exclusionPatterns) {
            if (entry.path().filename().string().find(pattern)) {
                return true;
            }
        }
        return false;
    };

    try {
        // Create dir if it doesn't exist
        if (!std::filesystem::exists(dest)) {
            std::filesystem::create_directory(dest);
        }
        // Iterate through each entry now
        for (const auto& entry : std::filesystem::recursive_directory_iterator(source)) {
            // Make sure it's a valid file that we should copy
            if (std::filesystem::is_regular_file(entry.path()) && entry.path().filename().string()[0] != '.' && !MatchesExclusionPattern(exclusionPatterns, entry)) {
                // Construct the corresponding path in the destination directory
                std::filesystem::path destPath = dest / entry.path().relative_path();
                // Create necessary directories in the destination path
                std::filesystem::create_directories(destPath.parent_path());
                // Copy the file
                std::filesystem::copy_file(entry.path(), destPath, std::filesystem::copy_options::overwrite_existing);
            }
        }
    } catch (const std::filesystem::filesystem_error& ex) {
        // TODO: print error
    }
}
