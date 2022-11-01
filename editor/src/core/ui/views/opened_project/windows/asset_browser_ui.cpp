#include "asset_browser_ui.h"
#include "../../../../asset_browser.h"

ImGuiHelper::Window OpenedProjectUI::Windows::GetAssetBrowserWindow() {
    ImGuiHelper::Window assetBrowserWindow = {
        .name = "Asset Browser",
        .open = nullptr,
        .windowFlags = ImGuiWindowFlags_NoResize,
        .callbackFunc = [] (ImGuiHelper::Context* context) {
            static AssetBrowser* assetBrowser = AssetBrowser::Get();
            FileNodeUtils::DisplayFileNodeTree(assetBrowser->fileCache.rootNode, true);
        },
        .position = ImVec2{ 100.0f, 200.0f },
        .size = ImVec2{ 400.0f, 300.0f },
    };
    return assetBrowserWindow;
}
