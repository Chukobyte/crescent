#include "imgui_helper.h"
#include "imgui_internal.h"

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
ImGuiHelper::InputText::InputText(const std::string &label, std::string &value, int labelIndex)
    : buffer(std::make_unique<char[]>(bufferSize + 1)),
      label(label),
      value(value) {
    internalLabel = "###" + std::to_string(labelIndex) + label;
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
    if (ImGui::InputText(inputText.GetInternalLabel(), inputText.buffer.get(), inputText.bufferSize, inputText.flags)) {
        inputText.value = inputText.GetValue();
    }
}

//--- Drag Int ---//
ImGuiHelper::DragInt::DragInt(std::string label, int& value, int labelIndex)
    : label(std::move(label)),
      value(value) {
    internalLabel = "##" + std::to_string(labelIndex) + this->label;
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
void ImGuiHelper::BeginWindow(const ImGuiHelper::Window &window) {
    if (window.position.has_value()) {
        ImGui::SetNextWindowPos(*window.position, ImGuiCond_Once);
    }
    if (window.size.has_value()) {
        ImGui::SetNextWindowSize(*window.size, ImGuiCond_Once);
    }
    ImGui::Begin(window.name, window.open, window.windowFlags);
    window.callbackFunc(context);
}

void ImGuiHelper::BeginWindowWithEnd(const ImGuiHelper::Window& window) {
    BeginWindow(window);
    ImGui::End();
    context->FlushPopups();
}

//--- DockSpace ---//
void ImGuiHelper::DockSpace::Build(int windowWidth, int windowHeight) {
    dockSpaceId = ImGui::GetID(id.c_str());
    ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f), dockSpaceFlags);
    ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight), ImGuiCond_Always);
    ImGui::Begin("DockSpaceWindow", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground
                );
    if (!hasBuilt) {
        ImGui::DockBuilderRemoveNode(dockSpaceId); // clear any previous layout
        ImGui::DockBuilderAddNode(dockSpaceId, dockNodeFlags);
        ImGui::DockBuilderSetNodeSize(dockSpaceId, size);

        ImGuiID dockRightId = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Right, 0.2f, nullptr, &dockSpaceId);
        ImGuiID dockLeftId = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Left, 0.2f, nullptr, &dockSpaceId);
        ImGuiID dockLeftDownId = ImGui::DockBuilderSplitNode(dockLeftId, ImGuiDir_Down, 0.3f, nullptr, &dockLeftId);
        ImGuiID dockDownId = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Down, 0.3f, nullptr, &dockSpaceId);

        for (auto& dockSpaceWindow : windows) {
            ImGuiID dockId;
            switch (dockSpaceWindow.position) {
            case DockSpacePosition::Main: {
                dockId = dockSpaceId;
                break;
            }
            case DockSpacePosition::Left: {
                dockId = dockLeftId;
                break;
            }
            case DockSpacePosition::Right: {
                dockId = dockRightId;
                break;
            }
            case DockSpacePosition::LeftDown: {
                dockId = dockLeftDownId;
                break;
            }
            case DockSpacePosition::Down: {
                dockId = dockDownId;
                break;
            }
            }
            ImGui::DockBuilderDockWindow(dockSpaceWindow.window.name, dockId);
        }
        ImGui::DockBuilderFinish(dockSpaceId);
        hasBuilt = true;
    }

//    ImGui::End();
}
