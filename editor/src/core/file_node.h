#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>

enum class FileNodeType {
    Invalid,
    File,
    Directory,
};

enum class FileNodeRegularFileType {
    Invalid,
    Texture,
    AudioSource,
//    PythonScript,
};

// A wrapper around filesystem path
struct FileNode {
    std::string GetRelativePath() const;
    [[nodiscard]] bool IsEmpty() const;
    static FileNodeRegularFileType GetRegularFileType(const std::string& fileName);

    std::filesystem::path path;
    FileNodeType type = FileNodeType::Invalid;
    unsigned int index = 0;
    FileNodeRegularFileType regularFileType = FileNodeRegularFileType::Invalid;
    std::vector<FileNode> directories;
    std::vector<FileNode> files;
};

class FileNodeCache {
    enum class LoadFlag {
        None = 0,
        Recursive = 1 << 0,
        IncludeExtensions = 1 << 1,
        All = Recursive | IncludeExtensions,
    };

public:
    [[nodiscard]] bool HasFilesWithExtension(const std::string& extension) const;
    void AddFile(const std::string& extension, const FileNode& fileNode);
    std::vector<FileNode> GetFilesWithExtension(const std::string& extension);
    void LoadRootNode(const std::string& filePath, LoadFlag loadFlag = LoadFlag::All);

    FileNode rootNode;
    std::unordered_map<std::string, std::vector<FileNode>> extensionToFileNodeMap;

private:
    void LoadFileNodeEntries(FileNode& fileNode, LoadFlag loadFlag);

    unsigned int nodeIndexCount = 0;
};
