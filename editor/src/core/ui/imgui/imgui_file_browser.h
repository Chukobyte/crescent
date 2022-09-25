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

    std::string name;
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
    std::function<void(const std::filesystem::path&)> onModeCompletedFunc = nullptr;
    std::optional<FileNode> selectedNode;
    bool hasJustOpened = false;
    FileNodeCache pathCache;
    std::filesystem::path lastDirectoryPath;
};

void BeginFileBrowser(FileBrowser& fileBrowser);

class FileBrowserPopupManager : public Singleton<FileBrowserPopupManager> {
  public:
    FileBrowserPopupManager(singleton) {}
    void QueueOpenPopop(FileBrowser* fileBrowser);
    void Flush();

  private:
    std::vector<FileBrowser*> framePopupModals;
    std::vector<FileBrowser*> popupModalsToOpen;
};
} // ImGuiHelper
