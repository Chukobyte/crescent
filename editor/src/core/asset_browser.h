#pragma once

#include "utils/singleton.h"

#include <vector>
#include <filesystem>

#include "SquidTasks/Task.h"

// A wrapper around filesystem path
enum class FileNodeType {
    Invalid,
    File,
    Directory,
};

struct FileNode {
    std::filesystem::path path;
    FileNodeType type = FileNodeType::Invalid;
    std::vector<FileNode> directories;
    std::vector<FileNode> files;
};

namespace FileNodeUtils {
void LoadFileNodeDirEntries(FileNode& fileNode);
void DisplayFileNodeTree(FileNode& fileNode, bool isRoot = false);
}

class AssetBrowser : public Singleton<AssetBrowser> {
  public:
    AssetBrowser(singleton) {}

    Task<> UpdateFileSystemCache();

    FileNode rootNode;

  private:
    void RefreshCache();
};
