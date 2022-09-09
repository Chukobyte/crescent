#include "project_manager_ui.h"

#include "../engine/src/core/utils/rbe_file_system_utils.h"
#include "../engine/src/core/utils/logger.h"

#include "imgui/imgui_helper.h"
#include "../editor_context.h"
#include "../project_properties.h"
#include "../scene/scene_manager.h"
#include "../utils/file_system_helper.h"
#include "../utils/helper.h"

static EditorContext* editorContext = EditorContext::Get();

void ProjectManagerUI::ProcessMenuBar() {}

void ProjectManagerUI::ProcessModalPopups() {
    static ImGuiHelper::PopupModal popupModal = {
        .name = "Exit To Project Manager Confirmation",
        .open = nullptr,
        .windowFlags = 0,
        .callbackFunc = [] (ImGuiHelper::Context* context) {
            if (ImGui::Button("Close")) {
                ImGui::CloseCurrentPopup();
            }
        },
        .position = ImVec2{ 100.0f, 100.0f },
        .size = ImVec2{ 200.0f, 200.0f },
    };
    ImGuiHelper::BeginPopupModal(popupModal);
}

void ProjectManagerUI::ProcessWindows() {
    int windowWidth = 0;
    int windowHeight = 0;
    SDL_GetWindowSize(editorContext->window, &windowWidth, &windowHeight);
    static ImGuiHelper::Window window = {
        .name = "Project Manager",
        .open = nullptr,
        .windowFlags = 0,
        .callbackFunc = [gameProperties = ProjectProperties::Get()] (ImGuiHelper::Context* context) {
            SceneManager* sceneManager = SceneManager::Get();
            auto LoadProject = [sceneManager, gameProperties, edContext = EditorContext::Get()](const char* projectPath) {
                if (FileSystemHelper::GetCurrentDirectory() != std::string(projectPath)) {
                    rbe_fs_chdir(projectPath);
                }
                edContext->projectState = EditorProjectState::OpenedProject;
                rbe_logger_debug("Opening project at directory = %s", projectPath);
                gameProperties->LoadPropertiesFromConfig("cre_config.py");
                gameProperties->PrintProperties();
                if (gameProperties->initialNodePath.empty()) {
                    sceneManager->selectedSceneFile = sceneManager->GenerateDefaultSceneNodeFile();
                } else {
                    if (!sceneManager->LoadSceneFromFile(gameProperties->initialNodePath.c_str())) {
                        rbe_logger_error("Failed to load scene from file at path '%s'", gameProperties->initialNodePath.c_str());
                    }
                }
            };

            if (ImGui::Button("Go To Test Fighter Project")) {
                LoadProject("test_games/fighter_test");
            }

            ImGui::Separator();
            // Manual project path
            static std::string openProjectPath;
            static ImGuiHelper::InputText openProjectPathInputText("Open Project Path", openProjectPath);
            ImGuiHelper::BeginInputText(openProjectPathInputText);
            // TODO: Validate path
            const std::string fullOpenProjectPath = Helper::RemoveExtensionFromFilePath("test_games/" + openProjectPath);
            if (ImGui::Button("Open Project") && !openProjectPath.empty() && FileSystemHelper::DoesDirectoryExist(fullOpenProjectPath)) {
                LoadProject(fullOpenProjectPath.c_str());
            }

            ImGui::Separator();
            ImGui::Text("Creates new project in 'test_games' folder...");
            // Name
            static std::string newProjectName;
            static ImGuiHelper::InputText newProjectNameInputText("New Project Name", newProjectName);
            ImGuiHelper::BeginInputText(newProjectNameInputText);
            // Path
            static std::string newProjectPath;
            static ImGuiHelper::InputText newProjectPathInputText("New Project Path", newProjectPath);
            ImGuiHelper::BeginInputText(newProjectPathInputText);
            // Create new project
            const std::string fullNewProjectPath = Helper::RemoveExtensionFromFilePath("test_games/" + newProjectPath);
            if (ImGui::Button("Create New Project") && !newProjectName.empty() && !newProjectPath.empty() && !FileSystemHelper::DoesDirectoryExist(newProjectPath)) {
                if (!FileSystemHelper::CreateDirectory(fullNewProjectPath)) {
                    rbe_logger_error("Failed to create directory at '%s'", newProjectPath.c_str());
                    return;
                }
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
