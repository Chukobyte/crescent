#pragma once

#include "imgui_helper.h"
#include "../../file_node.h"

namespace ImGuiHelper {
struct FileBrowser : public PopupModal {
public:


private:
    FileNodeCache pathCache;
};

void BeginFileBrowser(const FileBrowser& fileBrowser);
} // ImGuiHelper


