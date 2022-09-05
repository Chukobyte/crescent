#include "project_manager_ui.h"

#include "../engine/src/core/utils/rbe_file_system_utils.h"
#include "../engine/src/core/utils/logger.h"

#include "imgui/imgui_helper.h"
#include "../editor_context.h"
#include "../project_properties.h"
#include "../scene/scene_manager.h"

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
            auto LoadProject = [gameProperties, edContext = EditorContext::Get()](const char* projectPath) {
                rbe_fs_chdir(projectPath);
                edContext->projectState = EditorProjectState::OpenedProject;
                rbe_logger_debug("Opening test project at directory = %s", projectPath);
                gameProperties->LoadPropertiesFromConfig("cre_config.py");
                gameProperties->PrintProperties();
                if (!gameProperties->initialNodePath.empty()) {
                    static SceneManager* sceneManager = SceneManager::Get();
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
            static std::string projectPath;
            static ImGuiHelper::InputText projectPathInputText("Project Path", projectPath);
            ImGuiHelper::BeginInputText(projectPathInputText);
            // TODO: Validate path
            if (ImGui::Button("Load Project") && !projectPath.empty()) {
                LoadProject(projectPath.c_str());
            }

        },
        .position = ImVec2{ 150.0f, 100.0f },
        .size = ImVec2{ 400.0f, 300.0f },
    };
    ImGuiHelper::BeginWindowWithEnd(window);
}
