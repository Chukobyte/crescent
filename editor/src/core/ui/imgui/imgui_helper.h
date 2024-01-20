#pragma once

#include <string>
#include <functional>
#include <utility>
#include <vector>
#include <optional>
#include <memory>

#include "imgui.h"

#include "../../utils/singleton.h"

// TODO: Templatize and separate classes/structs into separate files

namespace ImGuiHelper {
static const std::string COMBO_BOX_LIST_NONE = "<none>";


class Context : public Singleton<Context> {
  public:
    Context(singleton) {}
};

using ImGuiHelperCallbackFunc = std::function<void(Context*)>;

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
    ImGuiHelperCallbackFunc callbackFunc = nullptr;
    std::optional<ImVec2> position;
    std::optional<ImVec2> size;
    bool hasRegistered = false;
};

struct InputText {
    InputText(const std::string& label, std::string& value, int labelIndex = 0);
    void SetValue(std::string value);
    [[nodiscard]] std::string GetValue() const;
    [[nodiscard]] bool HasValue() const;
    [[nodiscard]] const char* GetInternalLabel() const;

    std::string label;
    std::string& value;
    ImGuiInputTextFlags flags = ImGuiInputTextFlags_None;
    size_t bufferSize = 256;
    std::unique_ptr<char[]> buffer;

  private:
    std::string internalLabel;
};

struct DragInt {
    DragInt(std::string label, int& value, int labelIndex = 0);
    [[nodiscard]] const char* GetInternalLabel() const;

    std::string label;
    int& value;
    float valueSpeed = 1.0f;
    int valueMin = 1;
    int valueMax = 10000;

  private:
    std::string internalLabel;
};

struct DragFloat {
    DragFloat(std::string label, float& value, int labelIndex = 0);
    [[nodiscard]] const char* GetInternalLabel() const;

    std::string label;
    float& value;
    float valueSpeed = 1.0f;
    float valueMin = -999999.0f;
    float valueMax = 999999.0f;
    char format[8] = "%.2f";

  private:
    std::string internalLabel;
};

struct DragFloat2 {
    DragFloat2(std::string label, float* value, int labelIndex = 0);
    [[nodiscard]] const char* GetInternalLabel() const;

    std::string label;
    float* value;
    float valueSpeed = 1.0f;
    float valueMin = -999999.0f;
    float valueMax = 999999.0f;
    char format[8] = "%.2f";

  private:
    std::string internalLabel;
};

struct DragFloat4 {
    DragFloat4(std::string label, float* value, int labelIndex = 0);
    [[nodiscard]] const char* GetInternalLabel() const;

    std::string label;
    float* value;
    float valueSpeed = 1.0f;
    float valueMin = -999999.0f;
    float valueMax = 999999.0f;
    char format[8] = "%.2f";

  private:
    std::string internalLabel;
};

struct ColorEdit4 {
    ColorEdit4(std::string label, float* value, int labelIndex = 0);
    [[nodiscard]] const char* GetInternalLabel() const;

    std::string label;
    float* value;
    ImGuiColorEditFlags flags = ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaPreview;

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

struct ComboBox {
    ComboBox(std::string label, const std::vector<std::string>& items, std::function<void(const char* newItem)> onSelectionChangeCallback = nullptr, int labelIndex = 0);
    [[nodiscard]] const char* GetInternalLabel() const;
    [[nodiscard]] const char* GetSelectedItem() const;
    void SetSelected(const std::string& itemToSelect, bool executeCallbacks = true);

    std::string label;
    std::vector<std::string> items;
    int selectedIndex = 0;
    std::function<void(const char* newItem)> onSelectionChangeCallback = nullptr;

  private:
    std::string internalLabel;
};

struct AssetBrowserComboBox {
    AssetBrowserComboBox(std::string label, std::string inExtension, std::function<void(const char* newItem)> onSelectionChangeCallback = nullptr, int labelIndex = 0);
    ~AssetBrowserComboBox();
    [[nodiscard]] const char* GetInternalLabel() const;
    [[nodiscard]] const char* GetSelectedItem() const;
    void SetSelected(const std::string& itemToSelect, bool executeCallbacks = true);

    std::string label;
    std::string extension;
    std::vector<std::string> items;
    int selectedIndex = 0;
    std::function<void(const char* newItem)> onSelectionChangeCallback = nullptr;

    void RefreshListFromBrowser();

  private:
    std::string internalLabel;
    unsigned int assetBrowserHandle = 0;
};

struct TreeNode {
    std::string label;
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
    ImGuiHelperCallbackFunc callbackFunc = nullptr;
};

struct Window {
    std::string name;
    bool* open = nullptr;
    ImGuiWindowFlags windowFlags = 0;
    ImGuiHelperCallbackFunc callbackFunc = nullptr;
    std::optional<ImVec2> position;
    std::optional<ImVec2> size;
    ImGuiCond_ windowCond = ImGuiCond_Once;
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
    DockSpacePosition position = DockSpacePosition::Main;
};

struct DockSpace {
    std::string id = "";
    ImVec2 size = ImVec2(800, 600);
    std::function<void()> onMainWindowUpdateCallback = nullptr;
    std::vector<DockSpaceWindow> windows;
    ImGuiID dockSpaceId = 0;
    bool hasBuilt = false;

    void Run(bool runWindows);
};

class StaticPopupModalManager : public Singleton<StaticPopupModalManager> {
  public:
    StaticPopupModalManager(singleton) {}
    void QueueOpenPopop(PopupModal* popupModal);
    void Flush();

  private:
    std::vector<PopupModal*> framePopupModals;
    std::vector<PopupModal*> popupModalsToOpen;
};

void BeginMainMenuBar(const MenuBar& menuBar);
void BeginPopupModal(const PopupModal& popupModal);
void BeginInputText(const InputText& inputText);
bool BeginDragInt(const DragInt& dragInt);
bool BeginDragFloat(const DragFloat& dragFloat);
void BeginDragFloat2(const DragFloat2& dragFloat2);
void BeginDragFloat4(const DragFloat4& dragFloat4);
void BeginColorEdit4(const ColorEdit4& colorEdit4);
void BeginCheckBox(const CheckBox& checkBox);
void BeginComboBox(ComboBox& comboBox);
void BeginAssetBrowserComboBox(AssetBrowserComboBox& comboBox);
void BeginTreeNode(const TreeNode& treeNode);
void BeginWindow(const Window& window);
void BeginWindowWithEnd(const Window& window);
}
