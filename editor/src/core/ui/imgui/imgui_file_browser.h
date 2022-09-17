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

    std::string rootPath;
    Mode mode = Mode::OpenFile;
    std::vector<std::string> validExtensions;
    std::function<void(const FileNode&)> onModeCompletedFunc = nullptr;
    std::optional<FileNode> selectedNode;
    FileNodeCache pathCache;
};

void BeginFileBrowser(const FileBrowser& fileBrowser);
} // ImGuiHelper
