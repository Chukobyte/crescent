#pragma once

#include <functional>
#include <vector>
#include <optional>

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

struct Window {
    const char* name;
    bool* open = nullptr;
    ImGuiWindowFlags windowFlags = 0;
    ImGuiHelperCallbackFunc callbackFunc;
    std::optional<ImVec2> position;
    std::optional<ImVec2> size;
};

void BeginMainMenuBar(const MenuBar& menuBar);
void BeginPopupModal(const PopupModal& popupModal);
void BeginWindow(const Window& window);
}
