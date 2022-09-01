#include "imgui_helper.h"
#include "imgui_internal.h"

#include <utility>

#include "../engine/src/core/utils/logger.h"

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

//--- Drag Float ---//
ImGuiHelper::DragFloat::DragFloat(std::string label, float &value, int labelIndex)
    : label(std::move(label)),
      value(value) {
    internalLabel = "##" + std::to_string(labelIndex) + this->label;
}

const char* ImGuiHelper::DragFloat::GetInternalLabel() const {
    return internalLabel.c_str();
}

void ImGuiHelper::BeginDragFloat(const DragFloat& dragFloat) {
    if (!dragFloat.label.empty()) {
        ImGui::Text("%s", dragFloat.label.c_str());
        ImGui::SameLine();
    }
    ImGui::DragFloat(dragFloat.GetInternalLabel(), &dragFloat.value, dragFloat.valueSpeed, dragFloat.valueMin, dragFloat.valueMax, dragFloat.format);
}

//--- Drag Float 2 ---//
ImGuiHelper::DragFloat2::DragFloat2(std::string label, float* value, int labelIndex)
    : label(std::move(label)),
      value(value) {
    internalLabel = "##" + std::to_string(labelIndex) + this->label;
}

const char* ImGuiHelper::DragFloat2::GetInternalLabel() const {
    return internalLabel.c_str();
}

void ImGuiHelper::BeginDragFloat2(const DragFloat2& dragFloat2) {
    if (!dragFloat2.label.empty()) {
        ImGui::Text("%s", dragFloat2.label.c_str());
        ImGui::SameLine();
    }
    ImGui::DragFloat2(dragFloat2.GetInternalLabel(), dragFloat2.value, dragFloat2.valueSpeed, dragFloat2.valueMin, dragFloat2.valueMax, dragFloat2.format);
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

//--- TreeNode ---//
void ImGuiHelper::BeginTreeNode(const ImGuiHelper::TreeNode &treeNode) {
    if (ImGui::TreeNodeEx(treeNode.label.c_str(), treeNode.flags)) {
        if (treeNode.callbackFunc) {
            treeNode.callbackFunc(context);
        }
        ImGui::TreePop();
    }
}

//--- Window ---//
void ImGuiHelper::BeginWindow(const ImGuiHelper::Window& window) {
    if (window.position.has_value()) {
        ImGui::SetNextWindowPos(*window.position, window.windowCond);
    }
    if (window.size.has_value()) {
        ImGui::SetNextWindowSize(*window.size, window.windowCond);
    }
    ImGui::Begin(window.name.c_str(), window.open, window.windowFlags);
    if (window.callbackFunc) {
        window.callbackFunc(context);
    }
}

void ImGuiHelper::BeginWindowWithEnd(const ImGuiHelper::Window& window) {
    BeginWindow(window);
    ImGui::End();
    context->FlushPopups();
}

//--- DockSpace ---//
void ImGuiHelper::DockSpace::Run(bool runWindows) {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    dockSpaceId = ImGui::GetID(id.c_str());
    ImVec2 dockPosition = viewport->Pos;
    ImVec2 dockSize = viewport->Size;
    dockPosition.y += 10.0f;
    dockSize.y -= dockPosition.y;

    ImGui::SetNextWindowPos(dockPosition, ImGuiCond_Always);
    ImGui::SetNextWindowSize(dockSize, ImGuiCond_Always);
    ImGui::Begin("DockSpace Windows", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar |
                 ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDocking
                );
    ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
    if (!hasBuilt) {
        // Run windows to have them defined...
        for (const auto& dockSpaceWindow : windows) {
            ImGuiHelper::BeginWindowWithEnd(dockSpaceWindow.window);
        }

        ImGui::DockBuilderRemoveNode(dockSpaceId); // clear any previous layout
        ImGui::DockBuilderAddNode(dockSpaceId, ImGuiDockNodeFlags_DockSpace | ImGuiDockNodeFlags_PassthruCentralNode);
        ImGui::DockBuilderSetNodeSize(dockSpaceId, dockSize);
        ImGui::DockBuilderSetNodePos(dockSpaceId, dockPosition);

        ImGuiID dockRightId = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Right, 0.2f, nullptr, &dockSpaceId);
        ImGuiID dockLeftId = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Left, 0.2f, nullptr, &dockSpaceId);
        ImGuiID dockLeftDownId = ImGui::DockBuilderSplitNode(dockLeftId, ImGuiDir_Down, 0.3f, nullptr, &dockLeftId);
        ImGuiID dockDownId = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Down, 0.3f, nullptr, &dockSpaceId);

        for (const auto& dockSpaceWindow : windows) {
            ImGuiID dockId = dockSpaceId;
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
            ImGui::DockBuilderDockWindow(dockSpaceWindow.window.name.c_str(), dockId);
        }
        ImGui::DockBuilderFinish(dockSpaceId);
        hasBuilt = true;
    }

    if (runWindows) {
        for (const auto& dockSpaceWindow : windows) {
            ImGuiHelper::BeginWindowWithEnd(dockSpaceWindow.window);
        }
        ImGui::End();
    }
}

