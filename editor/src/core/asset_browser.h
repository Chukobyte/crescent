#pragma once

#include "utils/singleton.h"

#include <vector>
#include <filesystem>

#include "SquidTasks/Task.h"

// File Node

// A wrapper around filesystem path
enum class FileNodeType {
    Invalid,
    File,
    Directory,
};

struct FileNode {
    std::filesystem::path path;
    FileNodeType type = FileNodeType::Invalid;
    unsigned int index = 0;
    std::vector<FileNode> directories;
    std::vector<FileNode> files;
};

namespace FileNodeUtils {
void LoadFileNodeDirEntries(FileNode& fileNode, unsigned int& nodeIndex);
void DisplayFileNodeTree(FileNode& fileNode, bool isRoot = false);
}

// Asset Browser

class AssetBrowser : public Singleton<AssetBrowser> {
  public:
    AssetBrowser(singleton) {}

    Task<> UpdateFileSystemCache();

    FileNode rootNode;
    std::optional<FileNode> selectedFileNode;

  private:
    void RefreshCache();
};
