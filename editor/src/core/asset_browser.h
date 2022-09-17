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
    // TODO: Consider moving these into a utility, interface, etc...
    void RenameFile(const std::filesystem::path& oldPath, const std::string& newName);
    void DeleteFile(const std::filesystem::path& path);
    void CreateDirectory(const std::filesystem::path& path, const std::string& name);

    std::optional<FileNode> selectedFileNode;
    FileNodeCache fileCache;

  private:
    // For now assumes one time subscribe only with no unsubscriptions
    std::vector<AssetBrowserRefreshFunc> registerRefreshFuncs;
    bool refreshCacheQueued = false;
};
