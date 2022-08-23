#pragma once

#include <string>
#include <functional>
#include <vector>
#include <optional>
#include <memory>

#include "imgui.h"

#include "../../utils/singleton.h"

namespace ImGuiHelper {
class Context : public Singleton<Context> {
  public:
    Context(singleton) {}
    void OpenPopup(const char* popupId);
    void FlushPopups();

  private:
    std::vector<const char*> popupIds = {};
};

using ImGuiHelperCallbackFunc = std::function<void(Context* context)>;

struct MenuItem {
    const char* name;
    const char* keyboardShortcut;
    ImGuiHelperCallbackFunc callbackFunc;
};

struct Menu {
    const char* name;
    std::vector<MenuItem> menuItems;
};

struct MenuBar {
    const char* name;
    std::vector<Menu> menus;
};

struct PopupModal {
    const char* name;
    bool* open = nullptr;
    ImGuiWindowFlags windowFlags = 0;
    ImGuiHelperCallbackFunc callbackFunc;
    std::optional<ImVec2> position;
    std::optional<ImVec2> size;
};

struct InputText {
    InputText(const std::string& label, std::string& value, int labelIndex = 0);
    void SetValue(std::string value);
    std::string GetValue() const;
    const char* GetInternalLabel() const;

    std::string label;
    std::string &value;
    ImGuiInputTextFlags flags = ImGuiInputTextFlags_None;
    size_t bufferSize = 256;
    std::unique_ptr<char[]> buffer;

  private:
    std::string internalLabel;
};

struct DragInt {
    DragInt(std::string label, int& value, int labelIndex = 0);
    const char* GetInternalLabel() const;

    std::string label;
    int& value;
    float valueSpeed = 1.0f;
    int valueMin = 1;
    int valueMax = 10000;

  private:
    std::string internalLabel;
};

struct CheckBox {
    CheckBox(std::string  label, bool& value);
    [[nodiscard]] const char* GetInternalLabel() const;

    std::string label;
    bool& value;

  private:
    std::string internalLabel;
};

struct Window {
    const char* name;
    bool* open = nullptr;
    ImGuiWindowFlags windowFlags = 0;
    ImGuiHelperCallbackFunc callbackFunc;
    std::optional<ImVec2> position;
    std::optional<ImVec2> size;
};

enum class DockSpacePosition {
    Main,
    Left,
    Right,
    Down,
    LeftDown,
};

struct DockSpaceWindow {
    Window window;
    DockSpacePosition position;
};

struct DockSpace {
    std::string id = "";
    ImVec2 size = ImVec2(800, 600);
    std::vector<DockSpaceWindow> windows;
    ImGuiDockNodeFlags dockSpaceFlags = ImGuiDockNodeFlags_None;
    ImGuiDockNodeFlags dockNodeFlags = ImGuiDockNodeFlags_PassthruCentralNode;
    ImGuiID dockSpaceId;
    bool hasBuilt = false;

    void Build();
};

void BeginMainMenuBar(const MenuBar& menuBar);
void BeginPopupModal(const PopupModal& popupModal);
void BeginInputText(const InputText& inputText);
void BeginDragInt(const DragInt& dragInt);
void BeginCheckBox(const CheckBox& checkBox);
void BeginWindow(const Window& window);
void BeginWindowWithEnd(const Window& window);
}
