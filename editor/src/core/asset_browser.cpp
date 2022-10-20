#include "asset_browser.h"

#include "../seika/src/utils/logger.h"

#include "ui/imgui/imgui_helper.h"
#include "utils/file_system_helper.h"

using namespace Squid;

// TODO: Fix issue with not registering 'Right Click' logic because tree node is closed...
void FileNodeUtils::DisplayFileNodeTree(FileNode &fileNode, const bool isRoot) {
    static AssetBrowser* assetBrowser = AssetBrowser::Get();
    const ImGuiTreeNodeFlags dirFlags = isRoot ? ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_None;
    ImGuiTreeNodeFlags defaultFlags = dirFlags;
    defaultFlags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;
    if ((fileNode.type == FileNodeType::Directory && fileNode.IsEmpty()) || fileNode.type == FileNodeType::File) {
        defaultFlags |= ImGuiTreeNodeFlags_Leaf;
    }
    if (assetBrowser->selectedFileNode.has_value() && assetBrowser->selectedFileNode->index == fileNode.index) {
        defaultFlags |= ImGuiTreeNodeFlags_Selected;
    }
    const std::string treeNodeLabel = isRoot ? "res://" : fileNode.path.filename().string();
    ImGuiHelper::TreeNode treeNode = {
        .label = treeNodeLabel,
        .flags = defaultFlags,
        .callbackFunc = [fileNode, isRoot, treeNodeLabel] (ImGuiHelper::Context* context) {
            // Left Click
            if (ImGui::IsItemClicked()) {
                assetBrowser->selectedFileNode = fileNode;
            }

            // Right Click
            const std::string fileNodePopupId = "popup_" + treeNodeLabel + std::to_string(fileNode.index);
            ImGui::OpenPopupOnItemClick(fileNodePopupId.c_str(), ImGuiPopupFlags_MouseButtonRight);
            if (ImGui::BeginPopup(fileNodePopupId.c_str())) {
                assetBrowser->selectedFileNode = fileNode;
                if (fileNode.type == FileNodeType::Directory && ImGui::MenuItem("Create Directory")) {
                    static ImGuiHelper::PopupModal createDirPopup = {
                        .name = "Create Directory Menu",
                        .open = nullptr,
                        .windowFlags = 0,
                        .position = ImVec2{ 100.0f, 100.0f },
                        .size = ImVec2{ 250.0f, 100.0f },
                    };
                    createDirPopup.callbackFunc = [fileNode] (ImGuiHelper::Context* context) {
                        static std::string newDirName;
                        ImGuiHelper::InputText newDirNameText("Dir Name", newDirName);
                        ImGuiHelper::BeginInputText(newDirNameText);
                        if (ImGui::Button("Close")) {
                            newDirName.clear();
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("Ok") && !newDirName.empty()) {
                            assetBrowser->CreateDirectory(fileNode.path, newDirName);
                            newDirName.clear();
                            ImGui::CloseCurrentPopup();
                        }
                    };
                    ImGuiHelper::StaticPopupModalManager::Get()->QueueOpenPopop(&createDirPopup);
                }

                if (!isRoot) {
                    if (ImGui::MenuItem("Rename")) {
                        static ImGuiHelper::PopupModal renameFilePopup = {
                            .name = "Rename File",
                            .open = nullptr,
                            .windowFlags = 0,
                            .position = ImVec2{ 100.0f, 100.0f },
                            .size = ImVec2{ 250.0f, 100.0f },
                        };
                        renameFilePopup.callbackFunc = [fileNode] (ImGuiHelper::Context* context) {
                            static std::string newFileName;
                            ImGuiHelper::InputText newFileNameText("File Name", newFileName);
                            ImGuiHelper::BeginInputText(newFileNameText);
                            if (ImGui::Button("Close")) {
                                newFileName.clear();
                                ImGui::CloseCurrentPopup();
                            }
                            ImGui::SameLine();
                            if (ImGui::Button("Ok") && !newFileName.empty()) {
                                assetBrowser->RenameFile(fileNode.path, newFileName);
                                newFileName.clear();
                                ImGui::CloseCurrentPopup();
                            }
                        };
                        ImGuiHelper::StaticPopupModalManager::Get()->QueueOpenPopop(&renameFilePopup);
                    }

                    if (ImGui::MenuItem("Delete")) {
                        assetBrowser->DeleteFile(fileNode.path);
                    }
                }

                ImGui::EndPopup();
            }

            // Directories
            for (FileNode dirFileNode : fileNode.directories) {
                DisplayFileNodeTree(dirFileNode);
            }
            // Files
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
        // TODO: Figure out what the refresh rate should be...
//        co_await WaitSeconds(10.0f, EditorContext::Time);
        co_await WaitUntil([this] { return refreshCacheQueued; });
        RefreshCache();
        refreshCacheQueued = false;
    }
}

void AssetBrowser::RefreshCache() {
    fileCache.LoadRootNodeDir(FileSystemHelper::GetCurrentDir());
    for (auto& sub : refreshSubscribers) {
        sub.func(fileCache.rootNode);
    }
}

void AssetBrowser::QueueRefreshCache() {
    refreshCacheQueued = true;
}

void AssetBrowser::RenameFile(const std::filesystem::path& oldPath, const std::string& newName) {
    const std::filesystem::path parentPath = oldPath.parent_path();
    const std::filesystem::path newFilePath = parentPath / newName;
    se_logger_debug("old file path = %s, new file path = %s", oldPath.string().c_str(), newFilePath.string().c_str());
    std::error_code ec;
    std::filesystem::rename(oldPath, newFilePath, ec);
    if (ec.value() != 0) {
        se_logger_error("ec value = %d, message = %s", ec.value(), ec.message().c_str());
    } else {
        QueueRefreshCache();
    }
}

// TODO: Add more validation checks
void AssetBrowser::DeleteFile(const std::filesystem::path& path) {
    std::error_code ec;
    if (std::filesystem::is_directory(path)) {
        std::uintmax_t numberDeleted = std::filesystem::remove_all(path, ec);
        se_logger_debug("Number of files deleted from path '%s' = '%zu'", path.string().c_str(), numberDeleted);
    } else {
        std::filesystem::remove(path, ec);
    }
    if (ec.value() != 0) {
        se_logger_error("Error deleting from path '%s'!\nec value = %d, message = %s",
                        path.string().c_str(), ec.value(), ec.message().c_str());
    } else {
        QueueRefreshCache();
    }
}

void AssetBrowser::CreateDirectory(const std::filesystem::path& path, const std::string& name) {
    std::error_code ec;
    const std::filesystem::path fullDirPath = path / name;
    if (std::filesystem::is_directory(fullDirPath)) {
        se_logger_warn("Directory '%s' already exists, not creating!", fullDirPath.string().c_str());
        return;
    }
    std::filesystem::create_directory(fullDirPath, ec);
    if (ec.value() != 0) {
        se_logger_error("Create directory failed for asset path '%s'!\nec value = %d, message = %s",
                        fullDirPath.string().c_str(), ec.value(), ec.message().c_str());
    } else {
        QueueRefreshCache();
    }
}

AssetBrowserRefreshSubscriberHandle AssetBrowser::RegisterRefreshCallback(const AssetBrowserRefreshFunc& func) {
    const AssetBrowserRefreshSubscriber newSub = { handleIndex++, func };
    refreshSubscribers.emplace_back(newSub);
    return newSub.handle;
}

void AssetBrowser::UnregisterRefreshCallback(AssetBrowserRefreshSubscriberHandle handle) {
    refreshSubscribers.erase(std::remove_if(refreshSubscribers.begin(), refreshSubscribers.end(), [handle](const AssetBrowserRefreshSubscriber& sub) {
        return handle == sub.handle;
    }), refreshSubscribers.end());
}
