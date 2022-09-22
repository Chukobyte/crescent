#include "project_manager_ui.h"

#include "../engine/src/core/utils/rbe_file_system_utils.h"
#include "../engine/src/core/utils/logger.h"

#include "imgui/imgui_helper.h"
#include "imgui/imgui_file_browser.h"
#include "../editor_context.h"
#include "../project_properties.h"
#include "../scene/scene_manager.h"
#include "../utils/file_system_helper.h"

static EditorContext* editorContext = EditorContext::Get();

void ProjectManagerUI::ProcessMenuBar() {}

void ProjectManagerUI::ProcessModalPopups() {}

void ProjectManagerUI::ProcessWindows() {
    int windowWidth = 0;
    int windowHeight = 0;
    SDL_GetWindowSize(editorContext->window, &windowWidth, &windowHeight);
    static ImGuiHelper::Window window = {
        .name = "Project Manager",
        .open = nullptr,
        .windowFlags = 0,
        .callbackFunc = [] (ImGuiHelper::Context* context) {
            static auto sceneManager = SceneManager::Get();
            static auto gameProperties = ProjectProperties::Get();
            static auto editorContext = EditorContext::Get();
            static auto LoadProject = [](const char* projectPath) {
                if (FileSystemHelper::GetCurrentDir() != std::string(projectPath)) {
                    rbe_fs_chdir(projectPath);
                }
                gameProperties->projectPath = projectPath;
                editorContext->projectState = EditorProjectState::OpenedProject;
                rbe_logger_debug("Opening project at directory = %s", projectPath);
                gameProperties->LoadPropertiesFromConfig("cre_config.py");
                gameProperties->PrintProperties();
                // Update recently opened projects list
                editorContext->settings.AddToRecentlyLoadedProjectsList(gameProperties->gameTitle, projectPath);
                editorContext->settings.SaveSettings();

                // Load initial scene if it exists, if not create a new one
                if (gameProperties->initialNodePath.empty()) {
                    sceneManager->selectedSceneFile = sceneManager->GenerateDefaultSceneNodeFile();
                } else {
                    if (!sceneManager->LoadSceneFromFile(gameProperties->initialNodePath.c_str())) {
                        rbe_logger_error("Failed to load scene from file at path '%s'", gameProperties->initialNodePath.c_str());
                    }
                }
            };

            // Test Fighter Section
            if (ImGui::Button("Go To Test Fighter Project")) {
                LoadProject(std::string(editorContext->initialDir + "/test_games/fighter_test").c_str());
            }
            ImGui::Separator();

            // Open Project Section
            static bool openFileBrowser = false;
            if (ImGui::Button("Open Project")) {
                openFileBrowser = true;
            }
            static ImGuiHelper::FileBrowser openProjectFileBrowser = {
                .name = "Open Project Browser",
                .open = nullptr,
                .windowFlags = ImGuiWindowFlags_NoResize,
                .callbackFunc = nullptr,
                .position = ImVec2{ 100.0f, 100.0f },
                .size = ImVec2{ 600.0f, 320.0f },
                .rootPath = {},
                .mode = ImGuiHelper::FileBrowser::Mode::OpenFile,
                .validExtensions = { ".py" },
                .onModeCompletedFunc = [](const std::filesystem::path& fullPath) {
                    rbe_logger_debug("Opening project at file path = '%s'", fullPath.parent_path().generic_string().c_str());
                    LoadProject(fullPath.parent_path().generic_string().c_str());
                }
            };
            ImGuiHelper::BeginFileBrowser(openProjectFileBrowser);
            if (openFileBrowser) {
                openFileBrowser = false;
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

            // New Project Section
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
                    rbe_logger_debug("New project at file path = '%s'", fullPath.generic_string().c_str());
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
                rbe_fs_chdir(std::filesystem::path(fullNewProjectPath).string().c_str());
                // Create New Project Stuff
                gameProperties->ResetToDefault();
                gameProperties->gameTitle = newProjectName;
                FileSystemHelper::WriteFile("cre_config.py", ProjectProperties::GetDefaultProjectPropertyFileContent(newProjectName));
                LoadProject(fullNewProjectPath.c_str());
            }
        },
        .position = ImVec2{ 150.0f, 100.0f },
        .size = ImVec2{ 400.0f, 300.0f },
    };
    ImGuiHelper::BeginWindowWithEnd(window);
}
