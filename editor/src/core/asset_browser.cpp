#include "asset_browser.h"

#include "editor_context.h"
#include "ui/imgui/imgui_helper.h"
#include "utils/file_system_helper.h"
#include "../../../engine/src/core/utils/logger.h"

using namespace Squid;

//--- FileNode ---//
std::string FileNode::GetRelativePath() const {
    std::filesystem::path projectRootDir(FileSystemHelper::GetCurrentDirectory());
    std::filesystem::path relativePath = std::filesystem::relative(path, projectRootDir);
    return relativePath.generic_string();
}

//--- FileNodeUtils ---//
void FileNodeUtils::LoadFileNodeDirEntries(FileNode& fileNode, unsigned int& nodeIndex) {
    for (auto const& dir_entry : std::filesystem::directory_iterator{fileNode.path}) {
//        rbe_logger_debug("dir entry path: '%s'", dir_entry.path().string().c_str());
//        rbe_logger_debug("dir entry relative path: '%s'", std::filesystem::relative(dir_entry.path(), fileNode.path).string().c_str());
        const std::string& fileName = dir_entry.path().filename().string();
        if (fileName == "__pycache__" || fileName[0] == '.') {
            continue;
        }
        if (std::filesystem::is_directory(dir_entry)) {
            FileNode dirNode = { dir_entry.path(), FileNodeType::Directory, nodeIndex++ };
            LoadFileNodeDirEntries(dirNode, nodeIndex);
            fileNode.directories.emplace_back(dirNode);
        } else if (std::filesystem::is_regular_file(dir_entry)) {
            FileNode regularFileNode = { dir_entry.path(), FileNodeType::File, nodeIndex++, GetFileNodeRegularType(dir_entry.path().filename().string()) };
            fileNode.files.emplace_back(regularFileNode);
        }
    }
}

FileNodeRegularFileType FileNodeUtils::GetFileNodeRegularType(const std::string &fileName) {
    if (fileName.ends_with(".png")) {
        return FileNodeRegularFileType::Texture;
    } else if (fileName.ends_with(".wav")) {
        return FileNodeRegularFileType::AudioSource;
    }
//    else if (fileName.ends_with(".py")) {
//        return FileNodeRegularFileType::PythonScript;
//    }
    return FileNodeRegularFileType::Invalid;
}

void FileNodeUtils::DisplayFileNodeTree(FileNode &fileNode, const bool isRoot) {
    static AssetBrowser* assetBrowser = AssetBrowser::Get();
    const ImGuiTreeNodeFlags dirFlags = isRoot ? ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_None;
    ImGuiTreeNodeFlags defaultFlags = fileNode.type == FileNodeType::Directory ? dirFlags : ImGuiTreeNodeFlags_Leaf;
    if (assetBrowser->selectedFileNode.has_value() && assetBrowser->selectedFileNode->index == fileNode.index) {
        defaultFlags |= ImGuiTreeNodeFlags_Selected;
    }
    ImGuiHelper::TreeNode treeNode = {
        .label = isRoot ? "res://" : fileNode.path.filename().string(),
        .flags = defaultFlags,
        .callbackFunc = [fileNode] (ImGuiHelper::Context* context) {
            // Left Click
            if (ImGui::IsItemClicked()) {
                assetBrowser->selectedFileNode = fileNode;
            }
            // Files
            for (FileNode dirFileNode : fileNode.directories) {
                DisplayFileNodeTree(dirFileNode);
            }
            // Directories
            for (FileNode regularFileNode : fileNode.files) {
                DisplayFileNodeTree(regularFileNode);
            }
        }
    };
    ImGuiHelper::BeginTreeNode(treeNode);
}

//--- AssetBrowser ---//
Task<> AssetBrowser::UpdateFileSystemCache() {
    RefreshCache();
    while (true) {
        co_await WaitSeconds(10.0f, EditorContext::Time);
        // TODO: Uncomment once implementation of asset browser is finished
//        RefreshCache();
    }
}

void AssetBrowser::RefreshCache() {
    std::filesystem::path projectRootDir(FileSystemHelper::GetCurrentDirectory());
//    rbe_logger_debug("root name: %s", projectRootDir.root_name().c_str());
//    rbe_logger_debug("root path: %s", projectRootDir.string().c_str());

    rootNode.path = projectRootDir;
    rootNode.type = FileNodeType::Directory;
    unsigned int startingIndex = rootNode.index + 1;
    if (!selectedFileNode.has_value()) {
        selectedFileNode = rootNode;
    }
    FileNodeUtils::LoadFileNodeDirEntries(rootNode, startingIndex);
}
