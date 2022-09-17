#include "imgui_file_browser.h"

namespace {
void DisplayFileBrowser(const ImGuiHelper::FileBrowser& fileBrowser) {
    static auto CloseDisplayPopup = [] {
        ImGui::CloseCurrentPopup();
    };

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

void ImGuiHelper::BeginFileBrowser(const ImGuiHelper::FileBrowser& fileBrowser) {
    static ImGuiHelper::Context* context = ImGuiHelper::Context::Get();
    if (fileBrowser.position.has_value()) {
        ImGui::SetNextWindowPos(*fileBrowser.position, ImGuiCond_Once);
    }
    if (fileBrowser.size.has_value()) {
        ImGui::SetNextWindowSize(*fileBrowser.size, ImGuiCond_Once);
    }
    if (ImGui::BeginPopupModal(fileBrowser.name, fileBrowser.open, fileBrowser.windowFlags)) {
        DisplayFileBrowser(fileBrowser);
        if (fileBrowser.callbackFunc) {
            fileBrowser.callbackFunc(context);
        }
        ImGui::EndPopup();
    }
}
