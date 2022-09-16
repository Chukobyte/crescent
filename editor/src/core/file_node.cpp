#include "file_node.h"

#include "utils/file_system_helper.h"
#include "../engine/src/core/utils/logger.h"

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
    return std::vector<FileNode>();
}

void FileNodeCache::LoadRootNodeDir(const std::string& filePath, LoadFlag loadFlag) {
    rootNode.path = filePath;
    if (!std::filesystem::is_directory(rootNode.path)) {
        rbe_logger_error("Failed to load root dir node at file path '%s'", filePath.c_str());
        return;
    }
    nodeIndexCount = 0;
    rootNode.type = FileNodeType::Directory;
    rootNode.directories.clear();
    rootNode.files.clear();
    rootNode.index = nodeIndexCount;
    LoadFileNodeEntries(rootNode, loadFlag);
}

void FileNodeCache::LoadFileNodeEntries(FileNode &fileNode, LoadFlag loadFlag) {
    for (auto const& dir_entry : std::filesystem::directory_iterator{fileNode.path}) {
        const std::string& fileName = dir_entry.path().filename().string();
        // TODO: Add exclusion lists
        if (fileName == "__pycache__" || fileName[0] == '.') {
            continue;
        }
        if (std::filesystem::is_directory(dir_entry.path())) {
            FileNode dirNode = { dir_entry.path(), FileNodeType::Directory, nodeIndexCount++ };
            LoadFileNodeEntries(dirNode, loadFlag);
            fileNode.directories.emplace_back(dirNode);
        } else if (std::filesystem::is_regular_file(dir_entry.path())) {
            FileNode regularFileNode = { dir_entry.path(), FileNodeType::File, nodeIndexCount++, FileNode::GetRegularFileType(dir_entry.path().filename().string()) };
            fileNode.files.emplace_back(regularFileNode);
            const std::string extension = regularFileNode.path.extension().string();
            if (LoadFlag::IncludeExtensions >>= loadFlag) {
                AddFile(extension, fileNode);
            }
        }
    }
}
