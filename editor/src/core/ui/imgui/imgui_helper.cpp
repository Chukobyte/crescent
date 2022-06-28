#include "imgui_helper.h"

static ImGuiHelper::Context* context = ImGuiHelper::Context::Get();

void ImGuiHelper::Context::OpenPopup(const char* popupId) { popupIds.emplace_back(popupId); }

void ImGuiHelper::Context::FlushPopups() {
    for (const char* id : popupIds) {
        ImGui::OpenPopup(id);
    }
    popupIds.clear();
}

void ImGuiHelper::BeginMainMenuBar(const ImGuiHelper::MenuBar& menuBar) {
    // Create Menu Bar
    if (ImGui::BeginMainMenuBar()) {
        // Create Menus
        for (const ImGuiHelper::Menu& menu : menuBar.menus) {
            if (ImGui::BeginMenu(menu.name)) {
                // Create Menu Items
                for (const ImGuiHelper::MenuItem& menuItem : menu.menuItems) {
                    if (ImGui::MenuItem(menuItem.name, menuItem.keyboardShortcut)) {
                        menuItem.callbackFunc(context);
                    }
                }
                ImGui::EndMenu();
            }
        }
        ImGui::EndMainMenuBar();
        context->FlushPopups();
    }
}

void ImGuiHelper::BeginPopupModal(const ImGuiHelper::PopupModal& popupModal) {
    if (popupModal.position.has_value()) {
        ImGui::SetNextWindowPos(*popupModal.position, ImGuiCond_Once);
    }
    if (popupModal.size.has_value()) {
        ImGui::SetNextWindowSize(*popupModal.size, ImGuiCond_Once);
    }
    if (ImGui::BeginPopupModal(popupModal.name, popupModal.open, popupModal.windowFlags)) {
        popupModal.callbackFunc(context);
        ImGui::EndPopup();
        context->FlushPopups();
    }
}

void ImGuiHelper::BeginWindow(const ImGuiHelper::Window& window) {
    if (window.position.has_value()) {
        ImGui::SetNextWindowPos(*window.position, ImGuiCond_Once);
    }
    if (window.size.has_value()) {
        ImGui::SetNextWindowSize(*window.size, ImGuiCond_Once);
    }
    ImGui::Begin(window.name, window.open, window.windowFlags);
    window.callbackFunc(context);
    ImGui::End();
    context->FlushPopups();
}