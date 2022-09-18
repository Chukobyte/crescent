#pragma once

#include "imgui_helper.h"
#include "../../file_node.h"

namespace ImGuiHelper {
struct FileBrowser {
  public:
    enum class Mode {
        SelectDir,
        OpenFile,
        SaveFile,
    };

    const char* name;
    bool* open = nullptr;
    ImGuiWindowFlags windowFlags = 0;
    ImGuiHelperCallbackFunc callbackFunc = nullptr;
    std::optional<ImVec2> position;
    std::optional<ImVec2> size;
    bool hasRegistered = false;
    unsigned int id = 0;

    std::string rootPath;
    Mode mode = Mode::OpenFile;
    std::vector<std::string> validExtensions;
    std::function<void(const std::string&)> onModeCompletedFunc = nullptr;
    std::optional<FileNode> selectedNode;
    bool hasJustOpened = false;
    FileNodeCache pathCache;
};

void BeginFileBrowser(FileBrowser& fileBrowser);
} // ImGuiHelper
