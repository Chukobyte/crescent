#include "opened_project_ui.h"

#include "../engine/src/core/utils/rbe_file_system_utils.h"
#include "../engine/src/core/utils/logger.h"

#include "imgui/imgui_helper.h"
#include "../editor_context.h"
#include "../project_properties.h"
#include "../utils/helper.h"
#include "../file_creation/config_file_creator.h"

const char* CONFIG_FILE_NAME = "test_cre_config.py";

static EditorContext* editorContext = EditorContext::Get();

void OpenedProjectUI::ProcessMenuBar() {
    static ImGuiHelper::MenuBar menuBar = {
        .name = "FileMenuBar",
        .menus = {
            {
                .name = "File",
                .menuItems = {
                    {
                        .name = "Go To Project Manager",
                        .keyboardShortcut = "Ctrl+Shift+Q",
                        .callbackFunc = [] (ImGuiHelper::Context* context) {
                            context->OpenPopup("Confirm Go To Project Manager");
                            rbe_fs_chdir(editorContext->initialDir.c_str());
                            editorContext->projectState = EditorProjectState::ProjectManager;
                            rbe_logger_debug("Going back to project manager at path = %s", editorContext->initialDir.c_str());
                        },
                    },

                    {
                        .name = "Exit",
                        .keyboardShortcut = "Alt+F4",
                        .callbackFunc = [] (ImGuiHelper::Context* context) {
                            editorContext->isRunning = false;
                        },
                    },
                },
            },
            {
                .name = "Project",
                .menuItems = {
                    {
                        .name = "Settings",
                        .keyboardShortcut = "",
                        .callbackFunc = [] (ImGuiHelper::Context* context) {
                            context->OpenPopup("Project Settings Menu");
                        },
                    },
                    {
                        .name = "Input",
                        .keyboardShortcut = "",
                        .callbackFunc = [] (ImGuiHelper::Context* context) {
                            context->OpenPopup("Input Configurations");
                        },
                    },
                    {
                        .name = "Fonts",
                        .keyboardShortcut = "",
                        .callbackFunc = [] (ImGuiHelper::Context* context) {
                            context->OpenPopup("Font Configurations");
                        },
                    },
                },
            },
        }
    };
    ImGuiHelper::BeginMainMenuBar(menuBar);
}

void OpenedProjectUI::ProcessModalPopups() {
    static ImGuiHelper::PopupModal projectSettingsPopup = {
        .name = "Project Settings Menu",
        .open = nullptr,
        .windowFlags = 0,
        .callbackFunc = [gameProperties = ProjectProperties::Get()] (ImGuiHelper::Context* context) {
            if (ImGui::Button("Close")) {
                ConfigFileCreator::GenerateConfigFile(CONFIG_FILE_NAME, gameProperties);
                ImGui::CloseCurrentPopup();
            }

            static ImGuiHelper::InputText titleText("Title", gameProperties->gameTitle);
            static ImGuiHelper::InputText initialScenePathText("Initial Node Path", gameProperties->initialNodePath);
            static ImGuiHelper::DragInt windowWidthInt("Window Width", gameProperties->windowWidth);
            static ImGuiHelper::DragInt windowHeightInt("Window Height", gameProperties->windowHeight);
            static ImGuiHelper::DragInt targetFPSInt("Target FPS", gameProperties->targetFPS);
            static ImGuiHelper::CheckBox areCollidersVisibleCheckBox("Are Colliders Visible", gameProperties->areCollidersVisible);

            ImGuiHelper::BeginInputText(titleText);
            ImGuiHelper::BeginInputText(initialScenePathText);
            ImGuiHelper::BeginDragInt(windowWidthInt);
            ImGuiHelper::BeginDragInt(windowHeightInt);
            ImGuiHelper::BeginDragInt(targetFPSInt);
            ImGuiHelper::BeginCheckBox(areCollidersVisibleCheckBox);
        },
        .position = ImVec2{ 100.0f, 100.0f },
        .size = ImVec2{ 200.0f, 200.0f },
    };
    ImGuiHelper::BeginPopupModal(projectSettingsPopup);

    static ImGuiHelper::PopupModal InputConfigurationPopup = {
        .name = "Input Configurations",
        .open = nullptr,
        .windowFlags = 0,
        .callbackFunc = [gameProperties = ProjectProperties::Get()] (ImGuiHelper::Context* context) {
            if (ImGui::Button("Close")) {
                ConfigFileCreator::GenerateConfigFile(CONFIG_FILE_NAME, gameProperties);
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Add")) {
                ProjectInputAction defaultInputAction;
                gameProperties->inputs.actions.emplace_back(defaultInputAction);
            }

            ImGui::Separator();
            int actionIndexToDelete = -1;
            for (size_t i = 0; i < gameProperties->inputs.actions.size(); i++) {
                ProjectInputAction& inputAction = gameProperties->inputs.actions[i];

                ImGuiHelper::InputText nameText("Name", inputAction.name, i);
                ImGuiHelper::BeginInputText(nameText);

                ImGuiHelper::DragInt deviceId("Device Id", inputAction.deviceId, i);
                deviceId.valueMin = 0;
                deviceId.valueMax = 16;
                ImGuiHelper::BeginDragInt(deviceId);

                // Values
                ImGui::Text("Values:");
                ImGui::SameLine();
                if (ImGui::Button("+")) {
                    inputAction.values.emplace_back("");
                }
                int deletedValueIndex = -1;
                for (size_t valueIndex = 0; valueIndex < inputAction.values.size(); valueIndex++) {
                    std::string& value = inputAction.values[valueIndex];
                    ImGuiHelper::InputText valueText("", value, valueIndex);
                    ImGuiHelper::BeginInputText(valueText);
                    ImGui::SameLine();
                    const std::string buttonText = "-##" + std::to_string(valueIndex);
                    if (ImGui::Button(buttonText.c_str())) {
                        deletedValueIndex = valueIndex;
                    }
                }
                if (deletedValueIndex >= 0) {
                    inputAction.values.erase(inputAction.values.begin() + deletedValueIndex);
                }

                const std::string deleteText = "Delete##" + std::to_string(i);
                if (ImGui::Button(deleteText.c_str())) {
                    actionIndexToDelete = i;
                }

                ImGui::Separator();
            }

            if (actionIndexToDelete >= 0) {
                gameProperties->inputs.actions.erase(gameProperties->inputs.actions.begin() + actionIndexToDelete);
            }
        },
        .position = ImVec2{ 100.0f, 100.0f },
        .size = ImVec2{ 200.0f, 200.0f },
    };
    ImGuiHelper::BeginPopupModal(InputConfigurationPopup);

    static ImGuiHelper::PopupModal FontConfigurationPopup = {
        .name = "Font Configurations",
        .open = nullptr,
        .windowFlags = 0,
        .callbackFunc = [gameProperties = ProjectProperties::Get()] (ImGuiHelper::Context* context) {
            if (ImGui::Button("Close")) {
                ConfigFileCreator::GenerateConfigFile(CONFIG_FILE_NAME, gameProperties);
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Add")) {
                FontAsset defaultFontAsset = FontAsset();
                defaultFontAsset.file_path = "";
                defaultFontAsset.uid = "";
                defaultFontAsset.size = 16;
                gameProperties->assets.fonts.emplace_back(defaultFontAsset);
            }
            ImGui::Separator();
            int fontIndexToDelete = -1;
            for (size_t i = 0; i < gameProperties->assets.fonts.size(); i++) {
                auto& fontAsset = gameProperties->assets.fonts[i];
                ImGuiHelper::InputText filePath("File Path", fontAsset.file_path, i);
                ImGuiHelper::BeginInputText(filePath);

                ImGuiHelper::InputText uid("UID", fontAsset.uid, i);
                ImGuiHelper::BeginInputText(uid);

                ImGuiHelper::DragInt size("Size", fontAsset.size, i);
                ImGuiHelper::BeginDragInt(size);

                const std::string deleteText = "Delete##" + std::to_string(i);
                if (ImGui::Button(deleteText.c_str())) {
                    fontIndexToDelete = i;
                }
                ImGui::Separator();
            }
            if (fontIndexToDelete >= 0) {
                gameProperties->assets.fonts.erase(gameProperties->assets.fonts.begin() + fontIndexToDelete);
            }
        },
        .position = ImVec2{ 100.0f, 100.0f },
        .size = ImVec2{ 200.0f, 200.0f },
    };
    ImGuiHelper::BeginPopupModal(FontConfigurationPopup);
}

void OpenedProjectUI::ProcessWindows() {
    int windowWidth = 0;
    int windowHeight = 0;
    SDL_GetWindowSize(editorContext->window, &windowWidth, &windowHeight);
    static ImGuiHelper::Window sceneOutlinerWindow = {
        .name = "Scene Outliner",
        .open = nullptr,
        .windowFlags = 0,
        .callbackFunc = [] (ImGuiHelper::Context* context) {},
        .position = ImVec2{ 150.0f, 100.0f },
        .size = ImVec2{ 400.0f, 300.0f },
    };
    ImGuiHelper::BeginWindow(sceneOutlinerWindow);

    static ImGuiHelper::Window sceneViewWindow = {
        .name = "Scene View",
        .open = nullptr,
        .windowFlags = 0,
        .callbackFunc = [] (ImGuiHelper::Context* context) {},
        .position = ImVec2{ 300.0f, 100.0f },
        .size = ImVec2{ 400.0f, 300.0f },
    };
    ImGuiHelper::BeginWindow(sceneViewWindow);

    static ImGuiHelper::Window detailsWindow = {
        .name = "Details",
        .open = nullptr,
        .windowFlags = 0,
        .callbackFunc = [] (ImGuiHelper::Context* context) {},
        .position = ImVec2{ 400.0f, 100.0f },
        .size = ImVec2{ 400.0f, 300.0f },
    };
    ImGuiHelper::BeginWindow(detailsWindow);

    static ImGuiHelper::Window assetBrowserWindow = {
        .name = "Asset Browser",
        .open = nullptr,
        .windowFlags = 0,
        .callbackFunc = [] (ImGuiHelper::Context* context) {},
        .position = ImVec2{ 100.0f, 200.0f },
        .size = ImVec2{ 400.0f, 300.0f },
    };
    ImGuiHelper::BeginWindow(assetBrowserWindow);

    static ImGuiHelper::Window consoleWindow = {
        .name = "Console",
        .open = nullptr,
        .windowFlags = 0,
        .callbackFunc = [] (ImGuiHelper::Context* context) {},
        .position = ImVec2{ 200.0f, 200.0f },
        .size = ImVec2{ 400.0f, 300.0f },
    };
    ImGuiHelper::BeginWindow(consoleWindow);
}
