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
using AssetBrowserRefreshSubscriberHandle = unsigned int;

struct AssetBrowserRefreshSubscriber {
    AssetBrowserRefreshSubscriberHandle handle = 0; // No active handle should have the value of '0'
    AssetBrowserRefreshFunc func = nullptr;
};

class AssetBrowser : public Singleton<AssetBrowser> {
  public:
    AssetBrowser(singleton) {}
    Task<> UpdateFileSystemCache();
    AssetBrowserRefreshSubscriberHandle RegisterRefreshCallback(const AssetBrowserRefreshFunc& func);
    void UnregisterRefreshCallback(AssetBrowserRefreshSubscriberHandle handle);
    void RefreshCache();
    void QueueRefreshCache();
    // TODO: Consider moving these into a utility, interface, etc...
    void RenameFile(const std::filesystem::path& oldPath, const std::string& newName);
    void DeleteFile(const std::filesystem::path& path);
    void CreateDirectory(const std::filesystem::path& path, const std::string& name);

    std::optional<FileNode> selectedFileNode;
    FileNodeCache fileCache;

  private:
    std::vector<AssetBrowserRefreshSubscriber> refreshSubscribers;
    bool refreshCacheQueued = false;
    AssetBrowserRefreshSubscriberHandle handleIndex = 1;
};
