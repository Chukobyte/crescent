#include "project_manager_ui.h"

#include "../seika/src/utils/se_file_system_utils.h"
#include "../seika/src/utils/logger.h"

#include "../engine/src/core/engine_context.h"
#include "../engine/src/core/core_info.h"

#include "imgui/imgui_helper.h"
#include "imgui/imgui_file_browser.h"
#include "../editor_context.h"
#include "../project_properties.h"
#include "../asset_manager.h"
#include "../scene/scene_manager.h"
#include "../file_creation/config_file_creator.h"
#include "../math/bezier_curve.h"
#include "../utils/file_system_helper.h"
#include "../utils/console_logger.h"

static EditorContext* editorContext = EditorContext::Get();

#ifdef _DEBUG
static bool isImguiDemoEnabled = false;
#endif

void ProjectManagerUI::ProcessMenuBar() {
#ifdef _DEBUG
    ImGuiHelper::MenuBar menuBar = {
        .name = "SceneMenuBar",
        .menus = {
            {
                .name = "Debug",
                .menuItems = {
                    {
                        .name = "Toggle Imgui Demo",
                        .keyboardShortcut = "",
                        .callbackFunc = [] (ImGuiHelper::Context* context) {
                            isImguiDemoEnabled = !isImguiDemoEnabled;
                        },
                    }
                },
            }
        }
    };
    ImGuiHelper::BeginMainMenuBar(menuBar);
#endif
}

void ProjectManagerUI::ProcessModalPopups() {}

void ProjectManagerUI::ProcessWindows() {
    int windowWidth = 0;
    int windowHeight = 0;
    SDL_GetWindowSize(editorContext->window, &windowWidth, &windowHeight);
    static ImGuiHelper::Window window = {
        .name = "Project Manager",
        .open = nullptr,
        .windowFlags = ImGuiWindowFlags_NoResize,
        .callbackFunc = [] (ImGuiHelper::Context* context) {
            static auto* sceneManager = SceneManager::Get();
            static auto* gameProperties = ProjectProperties::Get();
            static auto* assetManager = AssetManager::Get();
            static auto* editorContext = EditorContext::Get();
            static auto LoadProject = [](const char* projectPath) {
                if (FileSystemHelper::GetCurrentDir() != std::string(projectPath)) {
                    se_fs_chdir(projectPath);
                }
                gameProperties->projectPath = projectPath;
                editorContext->projectState = EditorProjectState::OpenedProject;
                se_logger_debug("Opening project at directory = %s", projectPath);
                gameProperties->LoadPropertiesFromConfig("project.ccfg");
                gameProperties->PrintProperties();
                // Update recently opened projects list
                editorContext->settings.AddToRecentlyLoadedProjectsList(gameProperties->gameTitle, projectPath);
                editorContext->settings.Save();
                // Update asset manager fonts
                assetManager->RefreshFromProperties(gameProperties);
                // Add initial log entry
                static ConsoleLogger* consoleLogger = ConsoleLogger::Get();
                consoleLogger->Clear();
                consoleLogger->AddEntry("Crescent Engine v" + std::string(CRE_CORE_VERSION));
                // Load initial scene if it exists, if not create a new one
                if (gameProperties->initialNodePath.empty()) {
                    sceneManager->selectedSceneFile = sceneManager->GenerateDefaultSceneNodeFile();
                } else {
                    if (!sceneManager->LoadSceneFromFile(gameProperties->initialNodePath.c_str())) {
                        se_logger_error("Failed to load scene from file at path '%s'",
                                        gameProperties->initialNodePath.c_str());
                    }
                }
            };

            // Test Fighter Section
            if (ImGui::Button("Go To Test Fighter Project")) {
                LoadProject(std::string(editorContext->initialDir + "/" + DEFAULT_START_PROJECT_PATH).c_str());
            }
            ImGui::Separator();

            // Open Project Section
            static ImGuiHelper::FileBrowser openProjectFileBrowser = {
                .name = "Open Project Browser",
                .open = nullptr,
                .windowFlags = ImGuiWindowFlags_NoResize,
                .callbackFunc = nullptr,
                .position = ImVec2{ 100.0f, 100.0f },
                .size = ImVec2{ 600.0f, 320.0f },
                .rootPath = {},
                .mode = ImGuiHelper::FileBrowser::Mode::OpenFile,
                .validExtensions = { ".ccfg" },
                .onModeCompletedFunc = [](const std::filesystem::path& fullPath) {
                    se_logger_debug("Opening project at file path = '%s'",
                                    fullPath.parent_path().generic_string().c_str());
                    LoadProject(fullPath.parent_path().generic_string().c_str());
                }
            };
            ImGuiHelper::BeginFileBrowser(openProjectFileBrowser);
            if (ImGui::Button("Open Project")) {
                ImGui::OpenPopup(openProjectFileBrowser.name.c_str());
            }
            ImGui::Separator();

            // Recently Opened Projects Section
            if (!editorContext->settings.recentlyLoadedProjects.empty()) {
                ImGui::Text("Recently Opened Projects");
                for (size_t i = 0; i < editorContext->settings.recentlyLoadedProjects.size(); i++) {
                    const auto& project = editorContext->settings.recentlyLoadedProjects[i];
                    if (i > 0) {
                        ImGui::SameLine();
                    }
                    if (ImGui::Button(project.name.c_str())) {
                        LoadProject(project.fullPath.c_str());
                    }
                }
                ImGui::Separator();
            }

            // Name
            static std::string newProjectName;
            static ImGuiHelper::InputText newProjectNameInputText("New Project Name", newProjectName);
            ImGuiHelper::BeginInputText(newProjectNameInputText);
            // Path
            static std::string newProjectPath;
            static ImGuiHelper::InputText newProjectPathInputText("New Project Path", newProjectPath);
            ImGuiHelper::BeginInputText(newProjectPathInputText);
            ImGui::SameLine();
            static ImGuiHelper::FileBrowser newProjectFileBrowser = {
                .name = "New Project Browser",
                .open = nullptr,
                .windowFlags = ImGuiWindowFlags_NoResize,
                .callbackFunc = nullptr,
                .position = ImVec2{ 100.0f, 100.0f },
                .size = ImVec2{ 600.0f, 320.0f },
                .rootPath = {},
                .mode = ImGuiHelper::FileBrowser::Mode::SelectDir,
                .validExtensions = {},
                .onModeCompletedFunc = [](const std::filesystem::path& fullPath) {
                    se_logger_debug("New project at file path = '%s'", fullPath.generic_string().c_str());
                    newProjectPathInputText.SetValue(fullPath.generic_string());
                }
            };
            ImGuiHelper::BeginFileBrowser(newProjectFileBrowser);
            if (ImGui::Button("Browse")) {
                ImGui::OpenPopup(newProjectFileBrowser.name.c_str());
            }
            // Create new project
            const std::string newProjectPathText = newProjectPathInputText.GetValue();
            const std::string fullNewProjectPath = Helper::RemoveExtensionFromFilePath(newProjectPathText);
            if (ImGui::Button("Create New Project") && FileSystemHelper::DirectoryExistsAndIsEmpty(fullNewProjectPath)) {
                se_fs_chdir(std::filesystem::path(fullNewProjectPath).string().c_str());
                // Create New Project Stuff
                gameProperties->ResetToDefault();
                gameProperties->gameTitle = newProjectName;
                ConfigFileCreator::GenerateConfigFile(gameProperties);
                LoadProject(fullNewProjectPath.c_str());
            }
        },
        .position = ImVec2{ 100.0f, 100.0f },
        .size = ImVec2{ 600.0f, 300.0f },
    };
    ImGuiHelper::BeginWindowWithEnd(window);

    // Test curve editor
    static ImGuiHelper::Window curveWindow = {
        .name = "Curve Editor Window",
        .open = nullptr,
        .windowFlags = ImGuiWindowFlags_None,
        .callbackFunc = [] (ImGuiHelper::Context* context) {
            static const CubicBezierCurve curve = {
                .p0 = { 0.0f, 0.0f },
                .p1 = { 100.0f, 200.0f },
                .p2 = { 300.0f, 200.0f },
                .p3 = { 400.0f, 0.0f }
            };
            static ImVec2 p0 = { curve.p0.x, curve.p0.y };
            static ImVec2 p1 = { curve.p1.x, curve.p1.y };
            static ImVec2 p2 = { curve.p2.x, curve.p2.y };
            static ImVec2 p3 = { curve.p3.x, curve.p3.y };

            // Control Point Drag Floats
            ImGuiHelper::DragFloat2 p0DragFloat2("p0", (float*) &p0);
            ImGuiHelper::DragFloat2 p1DragFloat2("p1", (float*) &p1);
            ImGuiHelper::DragFloat2 p2DragFloat2("p2", (float*) &p2);
            ImGuiHelper::DragFloat2 p3DragFloat2("p3", (float*) &p3);
            ImGuiHelper::BeginDragFloat2(p0DragFloat2);
            ImGuiHelper::BeginDragFloat2(p1DragFloat2);
            ImGuiHelper::BeginDragFloat2(p2DragFloat2);
            ImGuiHelper::BeginDragFloat2(p3DragFloat2);

            ImGui::BeginChild("Curve", ImGui::GetContentRegionAvail(), false, ImGuiWindowFlags_HorizontalScrollbar);
            const ImVec2 windowPos = ImGui::GetWindowPos();
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            const ImVec2 windowP0 = {p0.x + windowPos.x, p0.y + windowPos.y };
            const ImVec2 windowP1 = {p1.x + windowPos.x, p1.y + windowPos.y };
            const ImVec2 windowP2 = {p2.x + windowPos.x, p2.y + windowPos.y };
            const ImVec2 windowP3 = {p3.x + windowPos.x, p3.y + windowPos.y };
            drawList->AddBezierCubic(windowP0, windowP1, windowP2, windowP3, ImColor(255, 0, 0), 20, 3);
            ImGui::EndChild();
        },
        .position = ImVec2{ 200.0f, 100.0f },
        .size = ImVec2{ 300.0f, 300.0f },
    };
    ImGuiHelper::BeginWindowWithEnd(curveWindow);

#ifdef _DEBUG
    if (isImguiDemoEnabled) {
        ImGui::ShowDemoWindow();
    }
#endif
}
