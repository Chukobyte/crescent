#include "menu_bar_ui.h"

#include "../seika/src/utils/se_file_system_utils.h"

#include "../../imgui/imgui_file_browser.h"
#include "../../../scene/scene_manager.h"
#include "../../../project_properties.h"
#include "../../../utils/file_system_helper.h"
#include "../../../file_creation/scene_file_creator.h"
#include "../../../editor_context.h"
#include "../../../asset_browser.h"
#include "../../../asset_manager.h"
#include "../../../file_creation/config_file_creator.h"
#include "../../../game_exporter.h"

ImGuiHelper::MenuBar OpenedProjectUI::MenuBar::GetMenuBar() {
    static ProjectProperties* projectProperties = ProjectProperties::Get();
    static EditorContext* editorContext = EditorContext::Get();
    ImGuiHelper::MenuBar menuBar = {
        .name = "SceneMenuBar",
        .menus = {
            {
                .name = "Scene",
                .menuItems = {
                    {
                        .name = "New Scene",
                        .keyboardShortcut = "Ctrl+N",
                        .callbackFunc = [] (ImGuiHelper::Context* context) {
                            static SceneManager* sceneManager = SceneManager::Get();
                            if (sceneManager->selectedSceneFile) {
                                if (sceneManager->selectedSceneFile->hasBeenSaved) {
                                    SceneNodeFile* newSceneNodeFile = sceneManager->GenerateDefaultSceneNodeFile();
                                    sceneManager->selectedSceneFile = newSceneNodeFile;
                                } else {
                                    sceneManager->ResetCurrentSceneNodeFile();
                                }
                            }
                            // TODO: Add popup modal to confirm new scene with unsaved changes
                        },
                    },
                    {
                        .name = "Open Scene",
                        .keyboardShortcut = "Ctrl+O",
                        .callbackFunc = [] (ImGuiHelper::Context* context) {
                            static ImGuiHelper::FileBrowser openSceneFileBrowser = {
                                .name = "Open Scene Browser",
                                .open = nullptr,
                                .windowFlags = ImGuiWindowFlags_NoResize,
                                .callbackFunc = nullptr,
                                .position = ImVec2{ 100.0f, 100.0f },
                                .size = ImVec2{ 600.0f, 320.0f },
                                .rootPath = {},
                                .mode = ImGuiHelper::FileBrowser::Mode::OpenFile,
                                .validExtensions = { ".cscn" },
                                .onModeCompletedFunc = [](const std::filesystem::path& fullPath) {
                                    const std::string relativePath = projectProperties->GetPathRelativeToProjectPath(fullPath.generic_string());
                                    se_logger_debug("Open scene at file path = '%s'", relativePath.c_str());

                                    static SceneManager* sceneManager = SceneManager::Get();
                                    const std::string validSceneFilePath = Helper::ConvertFilePathToFilePathExtension(relativePath, ".cscn");
                                    if (!validSceneFilePath.empty() && FileSystemHelper::DoesFileExist(validSceneFilePath)) {
                                        sceneManager->selectedSceneFile = sceneManager->LoadSceneFromFile(validSceneFilePath.c_str());
                                        SE_ASSERT_FMT(sceneManager->selectedSceneFile != nullptr, "selected scene node file at path '%s' is NULL!",
                                                      sceneManager->selectedSceneFile);
                                        sceneManager->selectedSceneNode = sceneManager->selectedSceneFile->rootNode;
                                    }
                                }
                            };
                            openSceneFileBrowser.rootPath = projectProperties->projectPath;
                            ImGuiHelper::FileBrowserPopupManager::Get()->QueueOpenPopop(&openSceneFileBrowser);
                        },
                    },
                    {
                        .name = "Save Scene",
                        .keyboardShortcut = "Ctrl+S",
                        .callbackFunc = [sceneManager = SceneManager::Get()] (ImGuiHelper::Context* context) {
                            if (SceneNodeFile* selectedSceneFile = sceneManager->selectedSceneFile) {
                                if (selectedSceneFile->hasBeenSaved) {
                                    SceneFileCreator::GenerateSceneFile(selectedSceneFile, selectedSceneFile->filePath.c_str());
                                } else {
                                    static ImGuiHelper::FileBrowser saveSceneFileBrowser = {
                                        .name = "Save Scene Browser",
                                        .open = nullptr,
                                        .windowFlags = ImGuiWindowFlags_NoResize,
                                        .callbackFunc = nullptr,
                                        .position = ImVec2{ 100.0f, 100.0f },
                                        .size = ImVec2{ 600.0f, 320.0f },
                                        .rootPath = {},
                                        .mode = ImGuiHelper::FileBrowser::Mode::SaveFile,
                                        .validExtensions = { ".cscn" },
                                        .onModeCompletedFunc = [](const std::filesystem::path& fullPath) {
                                            const std::string relativePath = projectProperties->GetPathRelativeToProjectPath(fullPath.generic_string());
                                            auto* selectedSceneFile = SceneManager::Get()->selectedSceneFile;
                                            const std::string validFullFilePath = Helper::ConvertFilePathToFilePathExtension(relativePath, ".cscn");
                                            selectedSceneFile->filePath = validFullFilePath;
                                            se_logger_debug("New project at file path = '%s'",
                                                            selectedSceneFile->filePath.c_str());
                                            SceneFileCreator::GenerateSceneFile(selectedSceneFile, validFullFilePath.c_str());
                                            selectedSceneFile->hasBeenSaved = true;
                                            AssetBrowser::Get()->RefreshCache();
                                        }
                                    };
                                    saveSceneFileBrowser.rootPath = projectProperties->projectPath;
                                    ImGuiHelper::FileBrowserPopupManager::Get()->QueueOpenPopop(&saveSceneFileBrowser);
                                }
                            }
                        },
                    },
                    {
                        .name = "Go To Project Manager",
                        .keyboardShortcut = "Ctrl+Shift+Q",
                        .callbackFunc = [] (ImGuiHelper::Context* context) {
                            se_fs_chdir(editorContext->initialDir.c_str());
                            editorContext->projectState = EditorProjectState::ProjectManager;
                            se_logger_debug("Going back to project manager at path = %s",
                                            editorContext->initialDir.c_str());
                            // TODO: Clean up scene manager and stuff...
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
                            static ImGuiHelper::PopupModal projectSettingsPopup = {
                                .name = "Project Settings Menu",
                                .open = nullptr,
                                .windowFlags = ImGuiWindowFlags_NoResize,
                                .callbackFunc = [] (ImGuiHelper::Context* context) {
                                    static bool justOpened = true;
                                    // TODO: Do more validation
                                    if (ImGui::Button("Close") && !projectProperties->gameTitle.empty()) {
                                        ConfigFileCreator::GenerateConfigFile(projectProperties);
                                        justOpened = true;
                                        ImGui::CloseCurrentPopup();
                                        return;
                                    }

                                    static ImGuiHelper::InputText titleText("Title", projectProperties->gameTitle);
                                    ImGuiHelper::BeginInputText(titleText);

                                    static ImGuiHelper::AssetBrowserComboBox initialSceneComboBox("Initial Node Path", ".cscn", [](const char* newItem) {
                                        projectProperties->initialNodePath = newItem;
                                        if (projectProperties->initialNodePath == ImGuiHelper::COMBO_BOX_LIST_NONE) {
                                            projectProperties->initialNodePath.clear();
                                        }
                                    });
                                    if (justOpened) {
                                        initialSceneComboBox.SetSelected(!projectProperties->initialNodePath.empty() ? projectProperties->initialNodePath : ImGuiHelper::COMBO_BOX_LIST_NONE);
                                        justOpened = false;
                                    }
                                    ImGuiHelper::BeginAssetBrowserComboBox(initialSceneComboBox);

                                    static ImGuiHelper::DragInt windowWidthInt("Window Width", projectProperties->windowWidth);
                                    ImGuiHelper::BeginDragInt(windowWidthInt);

                                    static ImGuiHelper::DragInt windowHeightInt("Window Height", projectProperties->windowHeight);
                                    ImGuiHelper::BeginDragInt(windowHeightInt);

                                    static ImGuiHelper::DragInt targetFPSInt("Target FPS", projectProperties->targetFPS);
                                    ImGuiHelper::BeginDragInt(targetFPSInt);

                                    static ImGuiHelper::CheckBox areCollidersVisibleCheckBox("Are Colliders Visible", projectProperties->areCollidersVisible);
                                    ImGuiHelper::BeginCheckBox(areCollidersVisibleCheckBox);
                                },
                                .position = ImVec2{ 100.0f, 100.0f },
                                .size = ImVec2{ 600.0f, 200.0f },
                            };
                            ImGuiHelper::StaticPopupModalManager::Get()->QueueOpenPopop(&projectSettingsPopup);
                        },
                    },
                    {
                        .name = "Input",
                        .keyboardShortcut = "",
                        .callbackFunc = [] (ImGuiHelper::Context* context) {
                            static ImGuiHelper::PopupModal inputConfigurationPopup = {
                                .name = "Input Configurations",
                                .open = nullptr,
                                .windowFlags = 0,
                                .callbackFunc = [] (ImGuiHelper::Context* context) {
                                    if (ImGui::Button("Close")) {
                                        ConfigFileCreator::GenerateConfigFile(projectProperties);
                                        ImGui::CloseCurrentPopup();
                                    }
                                    ImGui::SameLine();
                                    if (ImGui::Button("Add")) {
                                        ProjectInputAction defaultInputAction;
                                        projectProperties->inputs.actions.emplace_back(defaultInputAction);
                                    }

                                    ImGui::Separator();
                                    int actionIndexToDelete = -1;
                                    for (size_t i = 0; i < projectProperties->inputs.actions.size(); i++) {
                                        ProjectInputAction& inputAction = projectProperties->inputs.actions[i];

                                        ImGuiHelper::InputText nameText("Name", inputAction.name, (int) i);
                                        ImGuiHelper::BeginInputText(nameText);

                                        ImGuiHelper::DragInt deviceId("Device Id", inputAction.deviceId, (int) i);
                                        deviceId.valueMin = 0;
                                        deviceId.valueMax = 16;
                                        ImGuiHelper::BeginDragInt(deviceId);

                                        // Values
                                        ImGui::Text("Values:");
                                        ImGui::SameLine();
                                        const std::string addValueText = "+##input_value_" + std::to_string(i);
                                        if (ImGui::Button(addValueText.c_str())) {
                                            inputAction.values.emplace_back("");
                                        }
                                        int deletedValueIndex = -1;
                                        for (size_t valueIndex = 0; valueIndex < inputAction.values.size(); valueIndex++) {
                                            const int combinedIndex = ((int) i * 10) + valueIndex;
                                            std::string& value = inputAction.values[valueIndex];
                                            ImGuiHelper::InputText valueText("", value, combinedIndex);
                                            ImGuiHelper::BeginInputText(valueText);
                                            ImGui::SameLine();
                                            const std::string buttonText = "-##" + std::to_string(combinedIndex);
                                            if (ImGui::Button(buttonText.c_str())) {
                                                deletedValueIndex = (int) valueIndex;
                                            }
                                        }
                                        if (deletedValueIndex >= 0) {
                                            inputAction.values.erase(inputAction.values.begin() + deletedValueIndex);
                                        }

                                        const std::string deleteText = "Delete##" + std::to_string(i);
                                        if (ImGui::Button(deleteText.c_str())) {
                                            actionIndexToDelete = (int) i;
                                        }

                                        ImGui::Separator();
                                    }

                                    if (actionIndexToDelete >= 0) {
                                        projectProperties->inputs.actions.erase(projectProperties->inputs.actions.begin() + actionIndexToDelete);
                                    }
                                },
                                .position = ImVec2{ 100.0f, 100.0f },
                                .size = ImVec2{ 200.0f, 200.0f },
                            };
                            ImGuiHelper::StaticPopupModalManager::Get()->QueueOpenPopop(&inputConfigurationPopup);
                        },
                    },
                    {
                        .name = "Fonts",
                        .keyboardShortcut = "",
                        .callbackFunc = [] (ImGuiHelper::Context* context) {
                            static ImGuiHelper::PopupModal fontConfigurationPopup = {
                                .name = "Font Configurations",
                                .open = nullptr,
                                .windowFlags = 0,
                                .callbackFunc = [] (ImGuiHelper::Context* context) {
                                    if (ImGui::Button("Close")) {
                                        ConfigFileCreator::GenerateConfigFile(projectProperties);
                                        AssetManager::Get()->RefreshFromProperties(projectProperties);
                                        ImGui::CloseCurrentPopup();
                                    }
                                    ImGui::SameLine();
                                    if (ImGui::Button("Add")) {
                                        FontAsset defaultFontAsset = FontAsset();
                                        defaultFontAsset.file_path = "";
                                        defaultFontAsset.uid = "";
                                        defaultFontAsset.size = 16;
                                        projectProperties->assets.fonts.emplace_back(defaultFontAsset);
                                    }
                                    ImGui::Separator();
                                    // Fonts
                                    int fontIndexToDelete = -1;
                                    static std::vector<ImGuiHelper::AssetBrowserComboBox> fontPathComboBoxes;
                                    for (size_t i = 0; i < projectProperties->assets.fonts.size(); i++) {
                                        auto& fontAsset = projectProperties->assets.fonts[i];
                                        if (i >= fontPathComboBoxes.size()) {
                                            fontPathComboBoxes.emplace_back(ImGuiHelper::AssetBrowserComboBox("File Path", ".ttf", nullptr, i));
                                            fontPathComboBoxes[i].SetSelected(!fontAsset.file_path.empty() ? fontAsset.file_path : ImGuiHelper::COMBO_BOX_LIST_NONE);
                                        }
                                        fontPathComboBoxes[i].onSelectionChangeCallback = [i](const char* newItem) {
                                            auto& font = projectProperties->assets.fonts[i];
                                            font.file_path = newItem;
                                            if (font.file_path == ImGuiHelper::COMBO_BOX_LIST_NONE) {
                                                font.file_path.clear();
                                            }
                                        };
                                        ImGuiHelper::BeginAssetBrowserComboBox(fontPathComboBoxes[i]);

                                        ImGuiHelper::InputText uid("UID", fontAsset.uid, i);
                                        ImGuiHelper::BeginInputText(uid);

                                        ImGuiHelper::DragInt size("Size", fontAsset.size, i);
                                        ImGuiHelper::BeginDragInt(size);

                                        const std::string deleteText = "Delete##" + std::to_string(i);
                                        if (ImGui::Button(deleteText.c_str())) {
                                            fontIndexToDelete = (int) i;
                                        }
                                        ImGui::Separator();
                                    }
                                    if (fontIndexToDelete >= 0) {
                                        projectProperties->assets.fonts.erase(projectProperties->assets.fonts.begin() + fontIndexToDelete);
                                    }
                                },
                                .position = ImVec2{ 100.0f, 100.0f },
                                .size = ImVec2{ 500.0f, 200.0f },
                            };
                            ImGuiHelper::StaticPopupModalManager::Get()->QueueOpenPopop(&fontConfigurationPopup);
                        },
                    },
                },
            },
            {
                .name = "Editor",
                .menuItems = {
                    {
                        .name = "Editor Settings",
                        .keyboardShortcut = "",
                        .callbackFunc = [] (ImGuiHelper::Context* context) {
                            static ImGuiHelper::PopupModal editorSettingsPopup = {
                                .name = "Editor Settings Menu",
                                .open = nullptr,
                                .windowFlags = ImGuiWindowFlags_NoResize,
                                .callbackFunc = [] (ImGuiHelper::Context* context) {
                                    static bool resetLogLevelComboBoxes = true;
                                    static EditorSettings* editorSettings = &EditorContext::Get()->settings;
                                    static ImGuiHelper::ComboBox editorLogLevelComboBox("EditorLogLevel", { "debug", "info", "warn", "error" }, [](const char* newItem) {
                                        if (std::string(newItem) != editorSettings->GetEditorLogLevelString()) {
                                            editorSettings->SetEditorLogLevel(newItem);
                                        }
                                    });
                                    static ImGuiHelper::ComboBox gameLogLevelComboBox("GameLogLevel", { "debug", "info", "warn", "error" }, [](const char* newItem) {
                                        if (std::string(newItem) != editorSettings->GetGameLogLevelString()) {
                                            editorSettings->SetGameLogLevel(newItem);
                                        }
                                    });
                                    if (resetLogLevelComboBoxes) {
                                        editorLogLevelComboBox.SetSelected(editorSettings->GetEditorLogLevelString());
                                        gameLogLevelComboBox.SetSelected(editorSettings->GetGameLogLevelString());
                                        resetLogLevelComboBoxes = false;
                                    }

                                    if (ImGui::Button("Close")) {
                                        resetLogLevelComboBoxes = true;
                                        editorSettings->Save();
                                        ImGui::CloseCurrentPopup();
                                    }
                                    ImGuiHelper::BeginComboBox(editorLogLevelComboBox);
                                    ImGuiHelper::BeginComboBox(gameLogLevelComboBox);
                                },
                                .position = ImVec2{ 100.0f, 100.0f },
                                .size = ImVec2{ 600.0f, 200.0f },
                            };
                            ImGuiHelper::StaticPopupModalManager::Get()->QueueOpenPopop(&editorSettingsPopup);
                        },
                    },
                },
            },
            {
                .name = "Export",
                .menuItems = {
                    {
                        .name = "Export Game",
                        .keyboardShortcut = "",
                        .callbackFunc = [] (ImGuiHelper::Context* context) {
                            static ImGuiHelper::PopupModal projectSettingsPopup = {
                                .name = "Export Game Menu",
                                .open = nullptr,
                                .windowFlags = ImGuiWindowFlags_NoResize,
                                .callbackFunc = [] (ImGuiHelper::Context* context) {
                                    static std::string exportFileName;
                                    static ImGuiHelper::InputText exportPathInputText("Folder Path", exportFileName);
                                    if (ImGui::Button("Cancel")) {
                                        exportPathInputText.SetValue("");
                                        ImGui::CloseCurrentPopup();
                                    }
                                    ImGui::SameLine();
                                    if (ImGui::Button("Export") && !exportFileName.empty()) {
                                        exportFileName = Helper::RemoveExtensionFromFilePath(exportPathInputText.GetValue());
                                        const GameExporter::ExportProperties exportProps = {
                                            .gameTitle = projectProperties->gameTitle,
                                            .exportName = exportFileName,
                                            .exportPath = editorContext->GetProjectExportPath(),
                                            .projectPath = FileSystemHelper::GetCurrentDir(),
                                            .binPath = editorContext->GetEngineBinPath(),
                                            .tempPath = editorContext->GetProjectExportPath() + "/" + "tmp_cre"
                                        };
                                        GameExporter::Export(exportProps);
                                        exportPathInputText.SetValue("");
                                        ImGui::CloseCurrentPopup();
                                    }
                                    ImGui::Separator();

                                    ImGuiHelper::BeginInputText(exportPathInputText);
                                    ImGui::SameLine();
                                    static ImGuiHelper::FileBrowser exportFileBrowser = {
                                        .name = "Export Project Browser",
                                        .open = nullptr,
                                        .windowFlags = ImGuiWindowFlags_NoResize,
                                        .callbackFunc = nullptr,
                                        .position = ImVec2{ 100.0f, 100.0f },
                                        .size = ImVec2{ 600.0f, 320.0f },
                                        .rootPath = {},
                                        .mode = ImGuiHelper::FileBrowser::Mode::SelectDir,
                                        .validExtensions = {},
                                        .onModeCompletedFunc = [](const std::filesystem::path& fullPath) {
                                            const std::string exportPath = fullPath.generic_string();
                                            se_logger_debug("Setting project export path to '%s'", exportPath.c_str());
                                            exportPathInputText.SetValue(exportPath);
                                        }
                                    };
                                    ImGuiHelper::BeginFileBrowser(exportFileBrowser);
                                    if (ImGui::Button("Browse")) {
                                        ImGui::OpenPopup(exportFileBrowser.name.c_str());
                                    }
                                },
                                .position = ImVec2{ 100.0f, 100.0f },
                                .size = ImVec2{ 600.0f, 200.0f },
                            };
                            ImGuiHelper::StaticPopupModalManager::Get()->QueueOpenPopop(&projectSettingsPopup);
                        },
                    },
                },
            },
        }
    };
    return menuBar;
}

