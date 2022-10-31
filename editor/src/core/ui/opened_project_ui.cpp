#include "opened_project_ui.h"

#include "../seika/src/utils/se_file_system_utils.h"
#include "../seika/src/utils/logger.h"

#include "../engine/src/core/scene/scene_utils.h"

#include "views/opened_project/menu_bar_ui.h"
#include "views/opened_project/windows/asset_browser_ui.h"
#include "views/opened_project/windows/details_ui.h"
#include "views/opened_project/windows/scene_outliner_ui.h"
#include "views/opened_project/windows/scene_view_ui.h"

#include "imgui/imgui_helper.h"
#include "../editor_context.h"
#include "../project_properties.h"
#include "../asset_manager.h"
#include "../utils/file_system_helper.h"
#include "../utils/helper.h"
#include "../scene/scene_manager.h"
#include "../asset_browser.h"
#include "../utils/console_logger.h"
#include "../utils/process_runner/process_runner.h"
#include "../editor_callbacks.h"
#include "../game_exporter.h"
#include "imgui/imgui_file_browser.h"

const std::string COMBO_BOX_LIST_VALUE_NONE = "<none>";

static EditorContext* editorContext = EditorContext::Get();

void OpenedProjectUI::ProcessMenuBar() {
    static ImGuiHelper::MenuBar menuBar = OpenedProjectUI::MenuBar::GetMenuBar();
    ImGuiHelper::BeginMainMenuBar(menuBar);
}

void OpenedProjectUI::ProcessModalPopups() {
    static ImGuiHelper::StaticPopupModalManager* staticPopupModalManager = ImGuiHelper::StaticPopupModalManager::Get();
    staticPopupModalManager->Flush();
    static ImGuiHelper::FileBrowserPopupManager* fileBrowserPopupManager = ImGuiHelper::FileBrowserPopupManager::Get();
    fileBrowserPopupManager->Flush();
}

void OpenedProjectUI::ProcessWindows() {
    int windowWidth = 0;
    int windowHeight = 0;
    SDL_GetWindowSize(editorContext->window, &windowWidth, &windowHeight);

    // Create editor windows
    static ImGuiHelper::Window sceneOutlinerWindow = OpenedProjectUI::Windows::GetSceneOutlinerWindow();

    static ImGuiHelper::Window assetImportWindow = OpenedProjectUI::Windows::GetAssetBrowserWindow();

    static ImGuiHelper::Window sceneViewWindow = OpenedProjectUI::Windows::GetSceneViewWindow();

    static ImGuiHelper::Window detailsWindow = OpenedProjectUI::Windows::GetDetailsWindow();

    static ImGuiHelper::Window assetBrowserWindow = {
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

    static ImGuiHelper::Window consoleWindow = {
        .name = "Console",
        .open = nullptr,
        .windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar,
        .callbackFunc = [] (ImGuiHelper::Context* context) {
            static ConsoleLogger* consoleLogger = ConsoleLogger::Get();
            if (ImGui::Button("Clear")) {
                consoleLogger->Clear();
            }
            ImGui::Separator();
            ImGui::BeginChild("ConsoleLogWindow", ImGui::GetContentRegionAvail(), false, ImGuiWindowFlags_HorizontalScrollbar);
            ImGui::TextUnformatted(consoleLogger->GetText().c_str());
            ImGui::EndChild();
        },
        .position = ImVec2{ 200.0f, 200.0f },
        .size = ImVec2{ 400.0f, 300.0f },
    };

    static ImGuiHelper::DockSpace dockSpace = {
        .id = "DockSpace",
        .size = ImVec2((float) windowWidth, (float) windowHeight),
        .onMainWindowUpdateCallback = [] {
            static ConsoleLogger* consoleLogger = ConsoleLogger::Get();
            static std::shared_ptr<ConsoleLogCapture> processLogCapture;
            static ProcessRunner engineProcess;
            static bool wasProcessRunningLastFrame = false;
            const bool isProcessRunning = engineProcess.IsRunning();
            if (ImGui::Button(">") && !isProcessRunning) {
                if (!ProjectProperties::Get()->initialNodePath.empty()) {
                    // Clear console log
                    consoleLogger->Clear();
                    processLogCapture = consoleLogger->CaptureOutput();
                    if (!engineProcess.Start(editorContext->GetEngineBinaryPath(), editorContext->GetEngineBinaryProgramArgs())) {
                        se_logger_error("Failed to start engine process at path '%s'",
                                        editorContext->GetEngineBinaryPath().c_str());
                    }
                    se_logger_debug("Starting engine process at path '%s' with args '%s'",
                                    editorContext->GetEngineBinaryPath().c_str(),
                                    editorContext->GetEngineBinaryProgramArgs().c_str());
                } else {
                    static ImGuiHelper::PopupModal playErrorPopup = {
                        .name = "Play Error",
                        .open = nullptr,
                        .windowFlags = 0,
                        .callbackFunc = [] (ImGuiHelper::Context* context) {
                            ImGui::Text("Set initial node path first!");
                            if (ImGui::Button("Close")) {
                                ImGui::CloseCurrentPopup();
                            }
                        },
                        .position = ImVec2{ 100.0f, 100.0f },
                        .size = ImVec2{ 250.0f, 100.0f },
                    };
                    ImGuiHelper::StaticPopupModalManager::Get()->QueueOpenPopop(&playErrorPopup);
                }
            }
            if (!isProcessRunning && wasProcessRunningLastFrame) {
                processLogCapture.reset();
            }
            ImGui::SameLine();
            if (ImGui::Button("[]") && isProcessRunning) {
                engineProcess.Stop();
                processLogCapture.reset();
            }
            wasProcessRunningLastFrame = isProcessRunning;
        },
        .windows = {
            { .window = sceneViewWindow, .position = ImGuiHelper::DockSpacePosition::Main },
            { .window = sceneOutlinerWindow, .position = ImGuiHelper::DockSpacePosition::Left },
            { .window = assetImportWindow, .position = ImGuiHelper::DockSpacePosition::Left },
            { .window = detailsWindow, .position = ImGuiHelper::DockSpacePosition::Right },
            { .window = assetBrowserWindow, .position = ImGuiHelper::DockSpacePosition::LeftDown },
            { .window = consoleWindow, .position = ImGuiHelper::DockSpacePosition::Down }
        }
    };
    dockSpace.Run(true);
}

Task<> OpenedProjectUI::ManageOpenedProject() {
    EditorContext* edContext = EditorContext::Get();
    while (true) {

        co_await WaitUntil([edContext] { return editorContext->projectState == EditorProjectState::OpenedProject; });

        while (editorContext->projectState == EditorProjectState::OpenedProject) {
            co_await Suspend();
        }

        co_await Suspend();
    }
}
