#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>

#include "utils/helper.h"

enum class FileNodeType {
    Invalid,
    File,
    Directory,
};

enum class FileNodeRegularFileType {
    Invalid,
    Texture,
    AudioSource,
    PythonScript,
};

// A wrapper around filesystem path
struct FileNode {
    [[nodiscard]] std::string GetRelativePath() const;
    [[nodiscard]] bool IsEmpty() const;
    static FileNodeRegularFileType GetRegularFileType(const std::string& fileName);

    std::filesystem::path path{};
    FileNodeType type = FileNodeType::Invalid;
    unsigned int index = 0;
    FileNodeRegularFileType regularFileType = FileNodeRegularFileType::Invalid;
    std::vector<FileNode> directories{};
    std::vector<FileNode> files{};
};

class FileNodeCache {
  public:
    enum class LoadFlag {
        None = 0,
        Recursive = 1 << 0,
        IncludeExtensions = 1 << 1,
        All = Recursive | IncludeExtensions,
    };

    [[nodiscard]] bool HasFilesWithExtension(const std::string& extension) const;
    std::vector<FileNode> GetFilesWithExtension(const std::string& extension);
    void LoadRootNodeDir(const std::string& filePath, LoadFlag loadFlag = LoadFlag::All);
    FileNode rootNode;

    std::unordered_map<std::string, std::vector<FileNode>> extensionToFileNodeMap;

  private:
    void LoadFileNodeEntries(FileNode& fileNode, LoadFlag loadFlag);
    void AddFile(const std::string& extension, const FileNode& fileNode);

    unsigned int nodeIndexCount = 0;
};

GENERATE_ENUM_CLASS_OPERATORS(FileNodeCache::LoadFlag)
