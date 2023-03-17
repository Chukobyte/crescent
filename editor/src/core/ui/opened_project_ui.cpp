#include "opened_project_ui.h"

#include "../seika/src/utils/logger.h"

#include "views/opened_project/menu_bar_ui.h"
#include "views/opened_project/windows/asset_browser_ui.h"
#include "views/opened_project/windows/asset_import_ui.h"
#include "views/opened_project/windows/console_ui.h"
#include "views/opened_project/windows/details_ui.h"
#include "views/opened_project/windows/scene_outliner_ui.h"
#include "views/opened_project/windows/scene_view_ui.h"

#include "../editor_context.h"
#include "../project_properties.h"
#include "../asset_browser.h"
#include "../utils/console_logger.h"
#include "../utils/process_runner/process_runner.h"
#include "imgui/imgui_file_browser.h"
#include "imgui/imgui_helper.h"

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
    static EditorContext* editorContext = EditorContext::Get();
    int windowWidth = 0;
    int windowHeight = 0;
    SDL_GetWindowSize(editorContext->window, &windowWidth, &windowHeight);

    // Create editor windows
    static ImGuiHelper::Window sceneOutlinerWindow = OpenedProjectUI::Windows::GetSceneOutlinerWindow();
    static ImGuiHelper::Window assetImportWindow = OpenedProjectUI::Windows::GetAssetImportWindow();
    static ImGuiHelper::Window sceneViewWindow = OpenedProjectUI::Windows::GetSceneViewWindow();
    static ImGuiHelper::Window detailsWindow = OpenedProjectUI::Windows::GetDetailsWindow();
    static ImGuiHelper::Window assetBrowserWindow = OpenedProjectUI::Windows::GetAssetBrowserWindow();
    static ImGuiHelper::Window consoleWindow = OpenedProjectUI::Windows::GetConsoleWindow();

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

    // TODO: Make a window handler mechanism to spawn windows
    std::optional<CurveEditor>& curveEditor = MenuBar::Tools::GetCurveEditor();
    if (curveEditor.has_value()) {
        curveEditor->Begin();
    }
}
