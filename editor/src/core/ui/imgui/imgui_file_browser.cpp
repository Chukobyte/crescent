#include "imgui_file_browser.h"

namespace {
unsigned int selectedFileBrowserIndex = 0;

void DisplayFileBrowser(const ImGuiHelper::FileBrowser& fileBrowser) {
    static std::string pathText;
    static std::string selectedExtensionType;

    static auto CloseDisplayPopup = [] {
        pathText.clear();
        selectedFileBrowserIndex = 0;
        ImGui::CloseCurrentPopup();
    };

    static ImGuiHelper::InputText pathInputText("Path:", pathText);
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
        if (fileBrowser.hasJustOpened) {
            extensionList.clear();
            if (fileBrowser.validExtensions.empty()) {
                extensionList.emplace_back("*.*");
            } else {
                extensionList = fileBrowser.validExtensions;
            }
        }
        ImGuiHelper::BeginComboBox(extensionSelectionComboBox);
    }

    if (ImGui::Button("Cancel")) {
        CloseDisplayPopup();
    }
    ImGui::SameLine();
    switch (fileBrowser.mode) {
    case ImGuiHelper::FileBrowser::Mode::SelectDir: {
        if (ImGui::Button("Select")) {
            CloseDisplayPopup();
        }
        break;
    }
    case ImGuiHelper::FileBrowser::Mode::OpenFile: {
        if (ImGui::Button("Open")) {
            CloseDisplayPopup();
        }
        break;
    }
    case ImGuiHelper::FileBrowser::Mode::SaveFile: {
        if (ImGui::Button("Save")) {
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
    if (ImGui::BeginPopupModal(fileBrowser.name, fileBrowser.open, fileBrowser.windowFlags)) {
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
