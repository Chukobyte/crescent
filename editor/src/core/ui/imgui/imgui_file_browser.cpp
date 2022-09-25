#include "imgui_file_browser.h"

#include "../../utils/file_system_helper.h"

namespace {
unsigned int selectedFileBrowserIndex = 0;

//--- File Browser ---//
void DisplayFileBrowser(ImGuiHelper::FileBrowser& fileBrowser) {
    static std::string pathText;
    static ImGuiHelper::InputText pathInputText("Path:", pathText);
    static std::string selectedExtensionType;
    static bool reloadDirPathCache = false;
    static bool reloadExtensionList = false;

    static auto CloseDisplayPopup = [] {
        pathInputText.SetValue("");
//        selectedFileBrowserIndex = 0;
        ImGui::CloseCurrentPopup();
    };

    static std::filesystem::path lastDirectoryPath = FileSystemHelper::GetCurrentDir();

    const ImVec2 windowSize = ImGui::GetWindowSize();

    static std::string dirText;
    static ImGuiHelper::InputText dirInputText("Dir:", dirText);
    ImGui::PushItemWidth(500);
    ImGuiHelper::BeginInputText(dirInputText);

    // Show Files/Dirs Region
    ImGui::SetNextWindowContentSize(ImVec2(windowSize.x - 20, 0));
    ImGui::BeginChild("##ScrollingRegion", ImVec2(0, 200), true, ImGuiWindowFlags_None);

    if (fileBrowser.hasJustOpened || reloadDirPathCache) {
        const std::string lastDirPathText = lastDirectoryPath.generic_string();
        dirInputText.SetValue(lastDirPathText);
        fileBrowser.pathCache.LoadRootNodeDir(lastDirPathText, FileNodeCache::LoadFlag::IncludeExtensions);
        reloadDirPathCache = false;
    }

    const auto& mode = fileBrowser.mode;
    static unsigned int selectionIndex = 0;
    static unsigned int index = 0;
    index = 0;
    // Go to parent directory selectable
    // Skip if at the root
    if (lastDirectoryPath.has_relative_path()) {
        if (ImGui::Selectable("..", selectionIndex == index, ImGuiSelectableFlags_AllowDoubleClick | ImGuiSelectableFlags_DontClosePopups)) {
            selectionIndex = index;

            if(ImGui::IsMouseDoubleClicked(0)) {
                lastDirectoryPath = lastDirectoryPath.parent_path();
                reloadDirPathCache = true;
                reloadExtensionList = true;
            }
        }
        index++;
    }

    for (auto& dir : fileBrowser.pathCache.rootNode.directories) {
        const std::string dirPath = dir.path.filename().string();
        if (ImGui::Selectable(std::string(dirPath + "/").c_str(), selectionIndex == index, ImGuiSelectableFlags_AllowDoubleClick | ImGuiSelectableFlags_DontClosePopups)) {
            selectionIndex = index;
            if (mode == ImGuiHelper::FileBrowser::Mode::SelectDir) {
                pathInputText.SetValue(dirPath);
            }

            if(ImGui::IsMouseDoubleClicked(0)) {
                lastDirectoryPath = dir.path;
                reloadDirPathCache = true;
                reloadExtensionList = true;
            }
        }
        index++;
    }

    static auto HandleFile = [](const FileNode& fileNode, ImGuiHelper::FileBrowser::Mode mode) {
        const std::string filePath = fileNode.path.filename().string();
        if (ImGui::Selectable(filePath.c_str(), selectionIndex == index, ImGuiSelectableFlags_AllowDoubleClick | ImGuiSelectableFlags_DontClosePopups)) {
            selectionIndex = index;
            if (mode == ImGuiHelper::FileBrowser::Mode::OpenFile || mode == ImGuiHelper::FileBrowser::Mode::SaveFile) {
                pathInputText.SetValue(filePath);
            }

            if(ImGui::IsMouseDoubleClicked(0)) {}
        }
        index++;
    };
    if (mode != ImGuiHelper::FileBrowser::Mode::SelectDir) {
        if (fileBrowser.validExtensions.empty()) {
            for (auto& file : fileBrowser.pathCache.rootNode.files) {
                HandleFile(file, mode);
            }
        } else {
            for (auto& file : fileBrowser.pathCache.extensionToFileNodeMap[selectedExtensionType]) {
                HandleFile(file, mode);
            }
        }
    }

    ImGui::EndChild();
    ImGui::Separator();

    //Bottom Region
    ImGuiHelper::BeginInputText(pathInputText);

    if (fileBrowser.mode != ImGuiHelper::FileBrowser::Mode::SelectDir) {
        ImGui::SameLine();
        static std::vector<std::string> extensionList = { "*.*" };
        static ImGuiHelper::ComboBox extensionSelectionComboBox(
            "",
            extensionList,
        [](const char* newItem) {
            selectedExtensionType = newItem;
        }
        );
        if (fileBrowser.hasJustOpened  || reloadExtensionList) {
            extensionList.clear();
            if (fileBrowser.validExtensions.empty()) {
                extensionList.emplace_back("*.*");
            } else {
                extensionList = fileBrowser.validExtensions;
            }
            extensionSelectionComboBox.items = extensionList;
            selectedExtensionType = extensionList[0];
            reloadExtensionList = false;
        }
        ImGuiHelper::BeginComboBox(extensionSelectionComboBox);
    }

    if (ImGui::Button("Cancel")) {
        CloseDisplayPopup();
    }
    ImGui::SameLine();
    // TODO: Validate better...
    std::string pathCandidate = dirInputText.GetValue() + "/" + pathInputText.GetValue();
    if (!selectedExtensionType.empty() && mode != ImGuiHelper::FileBrowser::Mode::SelectDir && selectedExtensionType != "*.*") {
        pathCandidate = Helper::ConvertFilePathToFilePathExtension(pathCandidate, selectedExtensionType);
    } else if (mode == ImGuiHelper::FileBrowser::Mode::SelectDir) {
        pathCandidate = Helper::RemoveExtensionFromFilePath(pathCandidate);
    }
    const std::filesystem::path fullPath = pathCandidate;
    const bool doesPathInputHaveText = !pathCandidate.empty();
    switch (mode) {
    case ImGuiHelper::FileBrowser::Mode::SelectDir: {
        if (ImGui::Button("Open") && doesPathInputHaveText && FileSystemHelper::DoesDirectoryExist(fullPath)) {
            lastDirectoryPath = fullPath;
            reloadDirPathCache = true;
            reloadExtensionList = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Select") && doesPathInputHaveText && FileSystemHelper::DoesDirectoryExist(fullPath)) {
            if (fileBrowser.onModeCompletedFunc) {
                fileBrowser.onModeCompletedFunc(fullPath);
            }
            CloseDisplayPopup();
        }
        break;
    }
    case ImGuiHelper::FileBrowser::Mode::OpenFile: {
        if (ImGui::Button("Open") && doesPathInputHaveText && FileSystemHelper::DoesFileExist(fullPath)) {
            if (fileBrowser.onModeCompletedFunc) {
                fileBrowser.onModeCompletedFunc(fullPath);
            }
            CloseDisplayPopup();
        }
        break;
    }
    case ImGuiHelper::FileBrowser::Mode::SaveFile: {
        if (ImGui::Button("Save") && doesPathInputHaveText) {
            if (fileBrowser.onModeCompletedFunc) {
                // TODO: Make override popup if file already exists
                fileBrowser.onModeCompletedFunc(fullPath);
            }
            CloseDisplayPopup();
        }
        break;
    }
    }
}
} // namespace

void ImGuiHelper::BeginFileBrowser(ImGuiHelper::FileBrowser& fileBrowser) {
    static ImGuiHelper::Context* context = ImGuiHelper::Context::Get();

    if (!fileBrowser.hasRegistered) {
        static unsigned int fileBrowserIndex = 1;
        fileBrowser.id = fileBrowserIndex;
        fileBrowserIndex++;
        fileBrowser.hasRegistered = true;
    }

    if (fileBrowser.position.has_value()) {
        ImGui::SetNextWindowPos(*fileBrowser.position, ImGuiCond_Once);
    }
    if (fileBrowser.size.has_value()) {
        ImGui::SetNextWindowSize(*fileBrowser.size, ImGuiCond_Once);
    }
    if (ImGui::BeginPopupModal(fileBrowser.name.c_str(), fileBrowser.open, fileBrowser.windowFlags)) {
        if (selectedFileBrowserIndex != fileBrowser.id && !fileBrowser.hasJustOpened) {
            fileBrowser.hasJustOpened = true;
            selectedFileBrowserIndex = fileBrowser.id;
        }
        DisplayFileBrowser(fileBrowser);
        if (fileBrowser.callbackFunc) {
            fileBrowser.callbackFunc(context);
        }
        ImGui::EndPopup();
    }
    fileBrowser.hasJustOpened = false;
}

//--- Popup Manager ---//
void ImGuiHelper::FileBrowserPopupManager::QueueOpenPopop(FileBrowser* fileBrowser) {
    if (!fileBrowser->hasRegistered) {
        fileBrowser->hasRegistered = true;
        framePopupModals.emplace_back(fileBrowser);
    }
    popupModalsToOpen.emplace_back(fileBrowser);
}

void ImGuiHelper::FileBrowserPopupManager::Flush() {
    for (auto* popupModal : popupModalsToOpen) {
        ImGui::OpenPopup(popupModal->name.c_str());
    }
    popupModalsToOpen.clear();
    for (auto* popupModal : framePopupModals) {
        ImGuiHelper::BeginFileBrowser(*popupModal);
    }
}
