#include "asset_browser.h"

#include "editor_context.h"
#include "utils/file_system_helper.h"
#include "../../../engine/src/core/utils/logger.h"

using namespace Squid;

void FileNodeUtils::LoadFileNodeDirEntries(FileNode& fileNode) {
    for (auto const& dir_entry : std::filesystem::directory_iterator{fileNode.path}) {
//        rbe_logger_debug("dir entry path: '%s'", dir_entry.path().string().c_str());
        rbe_logger_debug("dir entry relative path: '%s'", std::filesystem::relative(dir_entry.path(), fileNode.path).string().c_str());
        if (std::filesystem::is_directory(dir_entry)) {
            FileNode dirNode = { dir_entry.path(), FileNodeType::Directory };
            LoadFileNodeDirEntries(dirNode);
            fileNode.directories.emplace_back(dirNode);
        } else if (std::filesystem::is_regular_file(dir_entry)) {
            FileNode regularFileNode = { dir_entry.path(), FileNodeType::File };
            fileNode.files.emplace_back(regularFileNode);
        }
    }
}

Task<> AssetBrowser::UpdateFileSystemCache() {
    while (true) {
//        co_await WaitSeconds(10.0f, EditorContext::Time);
        co_await WaitSeconds(3.0f, EditorContext::Time);

        RefreshCache();
    }
}

void AssetBrowser::RefreshCache() {
    std::filesystem::path projectRootDir(FileSystemHelper::GetCurrentDirectory());
    rbe_logger_debug("root name: %s", projectRootDir.root_name().c_str());
    rbe_logger_debug("root path: %s", projectRootDir.string().c_str());

    rootNode.path = projectRootDir;
    rootNode.type = FileNodeType::Directory;
    FileNodeUtils::LoadFileNodeDirEntries(rootNode);
}
