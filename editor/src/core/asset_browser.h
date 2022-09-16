#pragma once

#include "utils/singleton.h"

#include <vector>
#include <unordered_map>
#include <filesystem>

#include "SquidTasks/Task.h"

#include "file_node.h"

namespace FileNodeUtils {
void DisplayFileNodeTree(FileNode& fileNode, bool isRoot = false);
} // namespace FileNodeUtils

// Asset Browser

using AssetBrowserRefreshFunc = std::function<void(const FileNode& rootNode)>;

class AssetBrowser : public Singleton<AssetBrowser> {
  public:
    AssetBrowser(singleton) {}
    Task<> UpdateFileSystemCache();
    void RegisterRefreshCallback(const AssetBrowserRefreshFunc& func);
    void RefreshCache();
    void QueueRefreshCache();

    void RenameFile(const std::filesystem::path& oldPath, const std::string& newName);
    void DeleteFile(const std::filesystem::path& path);
    void CreateDirectory(const std::filesystem::path& path, const std::string& name);
//    void RunFuncOnAllNodeFiles(FileNode& node, std::function<bool(FileNode& currentFileNode)> func);
//    void RunFuncOnAllNodeDirs(FileNode& node, std::function<bool(FileNode& currentFileNode)> func);

//    FileNode rootNode;
    std::optional<FileNode> selectedFileNode;

//    std::unordered_map<std::string, std::vector<FileNode>> extensionToFileNodeMap;

    FileNodeCache fileCache;

  private:
    // For now assumes one time subscribe only with no unsubscriptions
    std::vector<AssetBrowserRefreshFunc> registerRefreshFuncs;
    bool refreshCacheQueued = false;
};
