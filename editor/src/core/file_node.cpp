#include <fstream>
#include "file_node.h"

#include "../seika/src/utils/logger.h"

#include "utils/file_system_helper.h"

//--- FileNode ---//
std::string FileNode::GetRelativePath() const {
    std::filesystem::path projectRootDir(FileSystemHelper::GetCurrentDir());
    std::filesystem::path relativePath = std::filesystem::relative(path, projectRootDir);
    return relativePath.generic_string();
}

bool FileNode::IsEmpty() const {
    return directories.empty() && files.empty();
}

FileNodeRegularFileType FileNode::GetRegularFileType(const std::string& fileName) {
    if (fileName.ends_with(".png")) {
        return FileNodeRegularFileType::Texture;
    } else if (fileName.ends_with(".wav")) {
        return FileNodeRegularFileType::AudioSource;
    } else if (fileName.ends_with(".py")) {
        return FileNodeRegularFileType::PythonScript;
    }
    return FileNodeRegularFileType::Invalid;
}

//--- FileNodeCache ---//
bool FileNodeCache::HasFilesWithExtension(const std::string& extension) const {
    return extensionToFileNodeMap.count(extension) > 0;
}

void FileNodeCache::AddFile(const std::string& extension, const FileNode& fileNode) {
    if (!HasFilesWithExtension(extension)) {
        extensionToFileNodeMap.emplace(extension, std::vector<FileNode> {});
    }
    extensionToFileNodeMap[extension].emplace_back(fileNode);
}

std::vector<FileNode> FileNodeCache::GetFilesWithExtension(const std::string& extension) {
    if (HasFilesWithExtension(extension)) {
        return extensionToFileNodeMap[extension];
    }
    return {};
}

void FileNodeCache::LoadRootNodeDir(const std::string& filePath, LoadFlag loadFlag) {
    rootNode.path = filePath;
    if (!std::filesystem::is_directory(rootNode.path)) {
        se_logger_error("Failed to load root dir node at file path '%s'", filePath.c_str());
        return;
    }
    nodeIndexCount = 0;
    rootNode.type = FileNodeType::Directory;
    rootNode.directories.clear();
    rootNode.files.clear();
    rootNode.index = nodeIndexCount++;
    extensionToFileNodeMap.clear();
    LoadFileNodeEntries(rootNode, loadFlag);
}

void FileNodeCache::LoadFileNodeEntries(FileNode &fileNode, LoadFlag loadFlag) {
    const bool isRecursive = LoadFlag::Recursive >>= loadFlag;
    const bool includeExtensions = LoadFlag::IncludeExtensions >>= loadFlag;
    for (auto const& dir_entry : std::filesystem::directory_iterator{fileNode.path}) {
        const std::string fileName = dir_entry.path().filename().string();
        // TODO: Add exclusion lists
        if (fileName == "__pycache__" || fileName[0] == '.') {
            continue;
        }
        std::error_code errorCode;
        if (std::filesystem::is_directory(dir_entry.path(), errorCode)) {
            FileNode dirNode = { dir_entry.path(), FileNodeType::Directory, nodeIndexCount++ };
            if (isRecursive) {
                LoadFileNodeEntries(dirNode, loadFlag);
            }
            fileNode.directories.emplace_back(dirNode);
        } else if (std::filesystem::is_regular_file(dir_entry.path(), errorCode)) {
            FileNode regularFileNode = { dir_entry.path(), FileNodeType::File, nodeIndexCount++, FileNode::GetRegularFileType(dir_entry.path().filename().string()) };
            fileNode.files.emplace_back(regularFileNode);
            if (includeExtensions) {
                const std::string extension = regularFileNode.path.extension().string();
                AddFile(extension, regularFileNode);
            }
        } else {
            // Can print out error code here if we want...
//            se_logger_debug("Not able to open up path '%s', skipping!  Error code: %s", dir_entry.path().c_str(), errorCode.message().c_str());
        }

    }
}
