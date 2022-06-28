#include "opened_project_ui.h"

#include "../engine/src/core/utils/logger.h"
#include "../engine/src/core/utils/rbe_file_system_utils.h"

#include "../editor_context.h"
#include "../project_properties.h"
#include "imgui/imgui_helper.h"

static EditorContext* editorContext = EditorContext::Get();

void OpenedProjectUI::ProcessMenuBar() {
    static ImGuiHelper::MenuBar menuBar = {
        .name  = "FileMenuBar",
        .menus = {
            {
                .name = "File",
                .menuItems =
                    {
                        {
                            .name             = "Project Settings",
                            .keyboardShortcut = "",
                            .callbackFunc =
                                [](ImGuiHelper::Context* context) { context->OpenPopup("Project Settings Menu"); },
                        },
                        {
                            .name             = "Go To Project Manager",
                            .keyboardShortcut = "Ctrl+Shift+Q",
                            .callbackFunc =
                                [editorContext](ImGuiHelper::Context* context) {
                                    context->OpenPopup("Confirm Go To Project Manager");
                                    rbe_fs_chdir(editorContext->initialDir.c_str());
                                    editorContext->projectState = EditorProjectState::ProjectManager;
                                    rbe_logger_debug("Going back to project manager at path = %s",
                                                     editorContext->initialDir.c_str());
                                },
                        },

                        {
                            .name             = "Exit",
                            .keyboardShortcut = "Alt+F4",
                            .callbackFunc =
                                [editorContext](ImGuiHelper::Context* context) { editorContext->isRunning = false; },
                        },
                    },
            },
        }};
    ImGuiHelper::BeginMainMenuBar(menuBar);
}

void OpenedProjectUI::ProcessModalPopups() {
    static ImGuiHelper::PopupModal popupModal = {
        .name        = "Project Settings Menu",
        .open        = nullptr,
        .windowFlags = 0,
        .callbackFunc =
            [gameProperties = ProjectProperties::Get()](ImGuiHelper::Context* context) {
                if (ImGui::Button("Close")) {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::Text(std::string("Game Title: " + gameProperties->gameTitle).c_str());
                ImGui::Text(std::string("Window Width: " + std::to_string(gameProperties->windowWidth)).c_str());
                ImGui::Text(std::string("Window Height: " + std::to_string(gameProperties->windowHeight)).c_str());
                ImGui::Text(std::string("Target FPS: " + std::to_string(gameProperties->targetFPS)).c_str());
            },
        .position = ImVec2{100.0f, 100.0f},
        .size     = ImVec2{200.0f, 200.0f},
    };
    //    popupModal.callbackFunc = [gameProperties = EditorGameProperties::Get()] (ImGuiHelper::Context* context) {
    //        if (ImGui::Button("Close")) {
    //            ImGui::CloseCurrentPopup();
    //        }
    //        ImGui::Text(std::string("Game Title: " + gameProperties->gameTitle).c_str());
    //        ImGui::Text(std::string("Window Width: " + std::to_string(gameProperties->windowWidth)).c_str());
    //        ImGui::Text(std::string("Window Height: " + std::to_string(gameProperties->windowHeight)).c_str());
    //        ImGui::Text(std::string("Target FPS: " + std::to_string(gameProperties->targetFPS)).c_str());
    //    };
    ImGuiHelper::BeginPopupModal(popupModal);
}

void OpenedProjectUI::ProcessWindows() {
    int windowWidth  = 0;
    int windowHeight = 0;
    SDL_GetWindowSize(editorContext->window, &windowWidth, &windowHeight);
    static ImGuiHelper::Window window = {
        .name         = "Project Selection",
        .open         = nullptr,
        .windowFlags  = 0,
        .callbackFunc = [](ImGuiHelper::Context* context) {},
        .position     = ImVec2{150.0f, 100.0f},
        .size         = ImVec2{400.0f, 300.0f},
    };
    ImGuiHelper::BeginWindow(window);
}
