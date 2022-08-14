#include "imgui_helper.h"

#include <utility>

static ImGuiHelper::Context* context = ImGuiHelper::Context::Get();

void ImGuiHelper::Context::OpenPopup(const char *popupId) {
    popupIds.emplace_back(popupId);
}

void ImGuiHelper::Context::FlushPopups() {
    for (const char* id : popupIds) {
        ImGui::OpenPopup(id);
    }
    popupIds.clear();
}

//--- MenuBar ---//
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

//--- Popup Modal ---//
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

//--- Input Text ---//
ImGuiHelper::InputText::InputText()
    : buffer(std::make_unique<char[]>(bufferSize + 1)) {}

ImGuiHelper::InputText::InputText(const std::string &label, const std::string &value)
    : buffer(std::make_unique<char[]>(bufferSize + 1)),
      label(label) {
    internalLabel = "###" + label;
    SetValue(value);
}

void ImGuiHelper::InputText::SetValue(std::string value) {
    if (value.size() > bufferSize) {
        value = value.substr(bufferSize);
    }
    std::ranges::copy(value, buffer.get());
    buffer[value.size()] = '\0';
}

std::string ImGuiHelper::InputText::GetValue() const {
    return { buffer.get() };
}

const char* ImGuiHelper::InputText::GetInternalLabel() const {
    return internalLabel.c_str();
}

void ImGuiHelper::BeginInputText(const InputText& inputText) {
    if (!inputText.label.empty()) {
        ImGui::Text("%s", inputText.label.c_str());
        ImGui::SameLine();
    }
    ImGui::InputText(inputText.GetInternalLabel(), inputText.buffer.get(), inputText.bufferSize, inputText.flags);
}

//--- Drag Int ---//
ImGuiHelper::DragInt::DragInt(std::string label, int& value)
    : label(std::move(label)),
      value(value) {
    internalLabel = "##" + this->label;
}

const char* ImGuiHelper::DragInt::GetInternalLabel() const {
    return internalLabel.c_str();
}

void ImGuiHelper::BeginDragInt(const DragInt& dragInt) {
    if (!dragInt.label.empty()) {
        ImGui::Text("%s", dragInt.label.c_str());
        ImGui::SameLine();
    }
    ImGui::DragInt(dragInt.GetInternalLabel(), &dragInt.value, dragInt.valueSpeed, dragInt.valueMin, dragInt.valueMax);
}

//--- CheckBox ---//
ImGuiHelper::CheckBox::CheckBox(std::string label, bool &value)
    : label(std::move(label)),
      value(value) {
    internalLabel = "##" + this->label;
}

const char* ImGuiHelper::CheckBox::GetInternalLabel() const {
    return internalLabel.c_str();
}

void ImGuiHelper::BeginCheckBox(const CheckBox& checkBox) {
    if (!checkBox.label.empty()) {
        ImGui::Text("%s", checkBox.label.c_str());
        ImGui::SameLine();
    }
    ImGui::Checkbox(checkBox.GetInternalLabel(), &checkBox.value);
}

//--- Window ---//
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