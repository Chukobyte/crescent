#include "opened_project_ui.h"

#include "../engine/src/core/scene/scene_utils.h"
#include "../engine/src/core/camera/camera.h"
#include "../engine/src/core/camera/camera_manager.h"
#include "../engine/src/core/utils/rbe_file_system_utils.h"
#include "../engine/src/core/utils/logger.h"

#include "imgui/imgui_helper.h"
#include "../editor_context.h"
#include "../project_properties.h"
#include "../asset_manager.h"
#include "../utils/file_system_helper.h"
#include "../utils/helper.h"
#include "../file_creation/config_file_creator.h"
#include "../scene/scene_manager.h"
#include "../asset_browser.h"
#include "../file_creation/scene_file_creator.h"
#include "../utils/console_logger.h"
#include "../utils/process_runner/process_runner.h"
#include "../editor_callbacks.h"
#include "../game_exporter.h"
#include "imgui/imgui_file_browser.h"
#include "imgui/imgui_window_renderer.h"

const std::string COMBO_BOX_LIST_NONE = "<none>";

static EditorContext* editorContext = EditorContext::Get();

void OpenedProjectUI::ProcessMenuBar() {
    static ProjectProperties* projectProperties = ProjectProperties::Get();
    static ImGuiHelper::MenuBar menuBar = {
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
                                    rbe_logger_debug("Open scene at file path = '%s'", relativePath.c_str());

                                    static SceneManager* sceneManager = SceneManager::Get();
                                    const std::string validSceneFilePath = Helper::ConvertFilePathToFilePathExtension(relativePath, ".cscn");
                                    if (!validSceneFilePath.empty() && FileSystemHelper::DoesFileExist(validSceneFilePath)) {
                                        sceneManager->selectedSceneFile = sceneManager->LoadSceneFromFile(validSceneFilePath.c_str());
                                        RBE_ASSERT_FMT(sceneManager->selectedSceneFile != nullptr, "selected scene node file at path '%s' is NULL!",
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
                                            rbe_logger_debug("New project at file path = '%s'", selectedSceneFile->filePath.c_str());
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
                            rbe_fs_chdir(editorContext->initialDir.c_str());
                            editorContext->projectState = EditorProjectState::ProjectManager;
                            rbe_logger_debug("Going back to project manager at path = %s", editorContext->initialDir.c_str());
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
                                        if (projectProperties->initialNodePath == COMBO_BOX_LIST_NONE) {
                                            projectProperties->initialNodePath.clear();
                                        }
                                    });
                                    if (justOpened) {
                                        initialSceneComboBox.SetSelected(!projectProperties->initialNodePath.empty() ? projectProperties->initialNodePath : COMBO_BOX_LIST_NONE);
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
                                            fontPathComboBoxes[i].SetSelected(!fontAsset.file_path.empty() ? fontAsset.file_path : COMBO_BOX_LIST_NONE);
                                        }
                                        fontPathComboBoxes[i].onSelectionChangeCallback = [i](const char* newItem) {
                                            auto& font = projectProperties->assets.fonts[i];
                                            font.file_path = newItem;
                                            if (font.file_path == COMBO_BOX_LIST_NONE) {
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
                                            rbe_logger_debug("Setting project export path to '%s'", exportPath.c_str());
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
    ImGuiHelper::BeginMainMenuBar(menuBar);
}

void OpenedProjectUI::ProcessModalPopups() {
    static ImGuiHelper::StaticPopupModalManager* staticPopupModalManager = ImGuiHelper::StaticPopupModalManager::Get();
    staticPopupModalManager->Flush();
    static ImGuiHelper::FileBrowserPopupManager* fileBrowserPopupManager = ImGuiHelper::FileBrowserPopupManager::Get();
    fileBrowserPopupManager->Flush();
}

// Temp
namespace ComponentDetailsDrawUtils {
void DrawTransform2D(SceneNode* node) {
    if (Transform2DComp* transform2DComp = node->GetComponentSafe<Transform2DComp>()) {
        ImGui::Text("Transform 2D Component");

        ImGuiHelper::DragFloat2 positionDragFloat2("Position", (float*) &transform2DComp->transform2D.position);
        ImGuiHelper::BeginDragFloat2(positionDragFloat2);

        ImGuiHelper::DragFloat2 scaleDragFloat2("Scale", (float*) &transform2DComp->transform2D.scale);
        ImGuiHelper::BeginDragFloat2(scaleDragFloat2);

        ImGuiHelper::DragFloat rotationDragFloat("Rotation", transform2DComp->transform2D.rotation);
        ImGuiHelper::BeginDragFloat(rotationDragFloat);

        ImGuiHelper::DragInt zIndexDragInt("Z Index", transform2DComp->zIndex);
        zIndexDragInt.valueMin = -1000;
        zIndexDragInt.valueMax = 1000;
        ImGuiHelper::BeginDragInt(zIndexDragInt);

        ImGuiHelper::CheckBox zIsRelativeToParentCheckBox("Z Is Relative To Parent", transform2DComp->isZIndexRelativeToParent);
        ImGuiHelper::BeginCheckBox(zIsRelativeToParentCheckBox);

        ImGuiHelper::CheckBox ignoreCameraCheckBox("Ignore Camera", transform2DComp->ignoreCamera);
        ImGuiHelper::BeginCheckBox(ignoreCameraCheckBox);

        ImGui::Separator();
    }
}

struct FuncObject {
    FuncObject(std::function<void()> func) {
        func();
    }
};

void DrawSprite(SceneNode* node) {
    if (SpriteComp* spriteComp = node->GetComponentSafe<SpriteComp>()) {
        ImGui::Text("Sprite Component");

        // Texture Path Combo Box
        static AssetBrowser* assetBrowser = AssetBrowser::Get();

        static ImGuiHelper::AssetBrowserComboBox spriteTexturePathComboBox("Texture Path", ".png");
        spriteTexturePathComboBox.onSelectionChangeCallback = [spriteComp](const char* newItem) {
            spriteComp->texturePath = newItem;
            if (spriteComp->texturePath == COMBO_BOX_LIST_NONE) {
                spriteComp->texturePath.clear();
            }
        };

        static FuncObject initializeFunc = FuncObject([spriteComp] {
            if (spriteComp->texturePath.empty()) {
                spriteTexturePathComboBox.SetSelected(COMBO_BOX_LIST_NONE);
            } else {
                spriteTexturePathComboBox.SetSelected(spriteComp->texturePath);
            }
            EditorCallbacks::Get()->RegisterOnSceneNodeSelected([](SceneNode* sceneNode) {
                spriteTexturePathComboBox.onSelectionChangeCallback = nullptr; // Disable on selection call back while switching nodes to prevent previous node from being set
                if (auto spriteC = sceneNode->GetComponentSafe<SpriteComp>()) {
                    if (spriteC->texturePath.empty()) {
                        spriteTexturePathComboBox.SetSelected(COMBO_BOX_LIST_NONE);
                    } else {
                        spriteTexturePathComboBox.SetSelected(spriteC->texturePath);
                    }
                }
            });
        });

        ImGuiHelper::BeginAssetBrowserComboBox(spriteTexturePathComboBox);

        // TODO: Update draw source automatically when a texture is set (can also make it toggleable)
        ImGuiHelper::DragFloat4 drawSourceDragFloat4("Draw Source", (float*) &spriteComp->drawSource);
        ImGuiHelper::BeginDragFloat4(drawSourceDragFloat4);

        ImGuiHelper::DragFloat2 originDragFloat2("Origin", (float*) &spriteComp->origin);
        ImGuiHelper::BeginDragFloat2(originDragFloat2);

        ImGuiHelper::ColorEdit4 modulateColorEdit4("Modulate", (float*) &spriteComp->modulate);
        ImGuiHelper::BeginColorEdit4(modulateColorEdit4);

        ImGuiHelper::CheckBox flipXCheckBox("Flix X", spriteComp->flipX);
        ImGuiHelper::BeginCheckBox(flipXCheckBox);

        ImGuiHelper::CheckBox flipYCheckBox("Flix Y", spriteComp->flipY);
        ImGuiHelper::BeginCheckBox(flipYCheckBox);

        ImGui::Separator();
    }
}

void DrawAnimatedSprite(SceneNode* node) {
    if (AnimatedSpriteComp* animatedSpriteComp = node->GetComponentSafe<AnimatedSpriteComp>()) {
        ImGui::Text("Animated Sprite Component");

        static std::vector<std::string> spriteAnimationList = { COMBO_BOX_LIST_NONE };
        static auto UpdateSpriteAnimationList = [] (AnimatedSpriteComp* animSpriteComp) {
            spriteAnimationList.clear();
            spriteAnimationList.emplace_back(COMBO_BOX_LIST_NONE);
            for (const auto& anim : animSpriteComp->animations) {
                spriteAnimationList.emplace_back(anim.name);
            }
        };
        UpdateSpriteAnimationList(animatedSpriteComp);
        static ImGuiHelper::ComboBox spriteAnimationSelectionComboBox("Current Animation", spriteAnimationList);
        spriteAnimationSelectionComboBox.items = spriteAnimationList;
        spriteAnimationSelectionComboBox.onSelectionChangeCallback = [animatedSpriteComp] (const char* newItem) {
            animatedSpriteComp->currentAnimationName = newItem;
            if (animatedSpriteComp->currentAnimationName == COMBO_BOX_LIST_NONE) {
                animatedSpriteComp->currentAnimationName.clear();
            }
        };
        ImGuiHelper::BeginComboBox(spriteAnimationSelectionComboBox);

        if (ImGui::Button("Edit Animations")) {
            static ImGuiHelper::PopupModal animationsEditPopup = {
                .name = "Animation Edit Menu",
                .open = nullptr,
                .windowFlags = 0,
                .position = ImVec2{ 100.0f, 100.0f },
                .size = ImVec2{ 400.0f, 400.0f },
            };
            static int selectedAnimIndex = 0;
            animationsEditPopup.callbackFunc = [animatedSpriteComp] (ImGuiHelper::Context* context) {
                static std::string selectedAnimName;
                static int selectedAnimFrameIndex = 0;
                static auto CleanupAnimEditState = [] {
                    selectedAnimIndex = 0;
                    selectedAnimFrameIndex = 0;
                    selectedAnimName.clear();
                };
                if (ImGui::Button("Close")) {
                    CleanupAnimEditState();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Ok")) {
                    CleanupAnimEditState();
                    ImGui::CloseCurrentPopup();
                }

                ImGui::Separator();

                // Animation List
                ImGui::Text("Animations:");
                if (ImGui::Button("+")) {
                    animatedSpriteComp->AddDefaultAnimation();
                }
                const ImGuiTreeNodeFlags defaultFlags = ImGuiTreeNodeFlags_Leaf;
                int animIndex = 0;
                struct QueuedAnimToRemove {
                    std::string name;
                    AnimatedSpriteComp* animSpriteComp = nullptr;
                };
                QueuedAnimToRemove queuedAnimToRemove;
                for (auto& anim : animatedSpriteComp->animations) {
                    ImGuiHelper::TreeNode treeNode = {
                        .label = anim.name,
                        .flags = animIndex == selectedAnimIndex ? defaultFlags | ImGuiTreeNodeFlags_Selected : defaultFlags,
                        .callbackFunc = [animatedSpriteComp, &anim, &animIndex, &queuedAnimToRemove] (ImGuiHelper::Context* context) {
                            if (animIndex == selectedAnimIndex) {
                                selectedAnimName = anim.name;
                            }
                            // Left Click
                            if (ImGui::IsItemClicked()) {
                                selectedAnimIndex = animIndex;
                                selectedAnimName = anim.name;
                            }

                            // Right Click
                            const std::string animPopupId = anim.name + "_popup";
                            ImGui::OpenPopupOnItemClick(animPopupId.c_str(), ImGuiPopupFlags_MouseButtonRight);
                            static ImGuiHelper::PopupModal renameAnimPopup = {
                                .name = "Rename Animation Menu",
                                .open = nullptr,
                                .windowFlags = 0,
                                .position = ImVec2{ 200.0f, 100.0f },
                                .size = ImVec2{ 200.0f, 200.0f },
                            };
                            bool shouldRenameAnim = false;
                            if (ImGui::BeginPopup(animPopupId.c_str())) {
                                selectedAnimIndex = animIndex;
                                selectedAnimName = anim.name;
                                if (ImGui::MenuItem("Rename")) {
                                    shouldRenameAnim = true;
                                    renameAnimPopup.callbackFunc = [animatedSpriteComp, &anim] (ImGuiHelper::Context* context) {
                                        static std::string newNameText;
                                        ImGuiHelper::InputText newNameInputText("New Name", newNameText);
                                        ImGuiHelper::BeginInputText(newNameInputText);

                                        if (ImGui::Button("Close")) {
                                            newNameText.clear();
                                            ImGui::CloseCurrentPopup();
                                        }
                                        ImGui::SameLine();
                                        if (ImGui::Button("Ok")) {
                                            if (!newNameText.empty() && !animatedSpriteComp->HasAnimationWithName(newNameText)) {
                                                anim.name = newNameText;
                                                selectedAnimName = anim.name;
                                            }
                                            newNameText.clear();
                                            ImGui::CloseCurrentPopup();
                                        }
                                    };
                                    // TODO: Should create a nested popup implementation
//                                    ImGuiHelper::StaticPopupModalManager::Get()->QueueOpenPopop(&renameAnimPopup);
                                }


                                if (ImGui::MenuItem("Delete")) {
                                    // Queue deletion
                                    selectedAnimIndex--;
                                    selectedAnimName.clear();
                                    queuedAnimToRemove.name = anim.name;
                                    queuedAnimToRemove.animSpriteComp = animatedSpriteComp;
                                }
                                ImGui::EndPopup();
                            }
                            if (shouldRenameAnim) {
                                ImGui::OpenPopup(renameAnimPopup.name);
                            }
                            ImGuiHelper::BeginPopupModal(renameAnimPopup);
                        }
                    };
                    ImGuiHelper::BeginTreeNode(treeNode);
                    animIndex++;
                }
                ImGui::Separator();

                // If there is a selected anim
                if (!selectedAnimName.empty() && animatedSpriteComp->HasAnimationWithName(selectedAnimName)) {
                    auto& selectedAnim = animatedSpriteComp->GetAnimationByName(selectedAnimName);
                    const size_t frameCount = selectedAnim.animationFrames.size();
                    ImGui::Text("Frame Count: %zu", frameCount);

                    // Have to define anim frame stuff here in order to refresh combo box when adding frame
                    static std::vector<std::string> animFrameTexturePathList = { COMBO_BOX_LIST_NONE };
                    static AssetBrowser* assetBrowser = AssetBrowser::Get();
                    static auto UpdateTexturePathList = [] {
                        animFrameTexturePathList.clear();
                        animFrameTexturePathList.emplace_back(COMBO_BOX_LIST_NONE);
                        if (assetBrowser->fileCache.extensionToFileNodeMap.count(".png") > 0) {
                            for (auto& fileNode : assetBrowser->fileCache.extensionToFileNodeMap[".png"]) {
                                animFrameTexturePathList.emplace_back(fileNode.GetRelativePath());
                            }
                        }
                    };
                    static FuncObject initializeAnimFrameTexturePathListFunc = FuncObject([] {
                        UpdateTexturePathList();
                        assetBrowser->RegisterRefreshCallback([](const FileNode& rootNode) {
                            UpdateTexturePathList();
                        });
                    });
                    static ImGuiHelper::ComboBox animFrameTexturePathComboBox("Texture Path", animFrameTexturePathList);

                    if (ImGui::Button("Add Frame")) {
                        EditorAnimationFrame newAnimFrame;
                        newAnimFrame.frame = frameCount;
                        selectedAnim.animationFrames.emplace_back(newAnimFrame);
                        selectedAnimFrameIndex = newAnimFrame.frame;
                        animFrameTexturePathComboBox.SetSelected(COMBO_BOX_LIST_NONE, false);
                    }

                    int animFrameToDelete = -1;
                    if (frameCount > 0) {
                        ImGui::SameLine();
                        if (ImGui::Button("Delete Frame")) {
                            animFrameToDelete = selectedAnimFrameIndex;
                            selectedAnimFrameIndex = Helper::Max(selectedAnimFrameIndex - 1, 0);
                        }

                        auto& selectedAnimFrame = selectedAnim.GetAnimationFrame(selectedAnimFrameIndex);

                        // TODO: Wrap asset browser file path lists in a UI Widget
                        // Anim Frame Texture Path
                        static FuncObject initializeFunc2 = FuncObject([selectedAnimFrame] {
                            if (selectedAnimFrame.texturePath.empty()) {
                                animFrameTexturePathComboBox.SetSelected(COMBO_BOX_LIST_NONE);
                            } else {
                                animFrameTexturePathComboBox.SetSelected(selectedAnimFrame.texturePath);
                            }
                        });
                        animFrameTexturePathComboBox.onSelectionChangeCallback = [animatedSpriteComp, animName = selectedAnim.name, animFrameIndex = selectedAnimFrame.frame](const char* newItem) {
                            if (animatedSpriteComp->HasAnimationWithName(animName)) {
                                auto& anim = animatedSpriteComp->GetAnimationByName(animName);
                                if (anim.HasAnimationFrame(animFrameIndex)) {
                                    auto& animFrame = anim.GetAnimationFrame(animFrameIndex);
                                    animFrame.texturePath = newItem;
                                    if (animFrame.texturePath == COMBO_BOX_LIST_NONE) {
                                        animFrame.texturePath.clear();
                                    }
                                }
                            }
                        };
                        ImGuiHelper::BeginComboBox(animFrameTexturePathComboBox);

                        // Draw Source
                        ImGuiHelper::DragFloat4 frameDrawSourceDragFloat4("Draw Source", (float*) &selectedAnimFrame.drawSource);
                        ImGuiHelper::BeginDragFloat4(frameDrawSourceDragFloat4);

                        // Selection Arrows
                        const int beforeArrowsAnimFrame = selectedAnimFrameIndex;
                        if (ImGui::Button("<--")) {
                            selectedAnimFrameIndex = Helper::Max(selectedAnimFrameIndex - 1, 0);
                        }
                        ImGui::SameLine();
                        ImGui::Text("Current Frame: %d", selectedAnimFrameIndex);
                        ImGui::SameLine();
                        if (ImGui::Button("-->")) {
                            selectedAnimFrameIndex = Helper::Min(selectedAnimFrameIndex + 1, (int) frameCount - 1);
                        }
                        if (beforeArrowsAnimFrame != selectedAnimFrameIndex) {
                            const auto& newSelectedAnimFrame = selectedAnim.GetAnimationFrame(selectedAnimFrameIndex);
                            const std::string newSelectedAnimFrameTexturePath = !newSelectedAnimFrame.texturePath.empty() ? newSelectedAnimFrame.texturePath : COMBO_BOX_LIST_NONE;
                            // Null callback since it's not needed and will be reset next frame
                            animFrameTexturePathComboBox.onSelectionChangeCallback = nullptr;
                            animFrameTexturePathComboBox.SetSelected(newSelectedAnimFrameTexturePath);
                        }
                    }
                    if (animFrameToDelete >= 0) {
                        selectedAnim.RemoveAnimatationFrameByIndex(animFrameToDelete);
                    }

                    ImGui::Separator();

                    ImGuiHelper::DragInt speedDragInt("Speed", selectedAnim.speed);
                    ImGuiHelper::BeginDragInt(speedDragInt);

                    ImGuiHelper::CheckBox loopsCheckBox("Loops", selectedAnim.doesLoop);
                    ImGuiHelper::BeginCheckBox(loopsCheckBox);
                }

                // Cleanup
                if (!queuedAnimToRemove.name.empty() && queuedAnimToRemove.animSpriteComp != nullptr) {
                    queuedAnimToRemove.animSpriteComp->RemoveAnimationByName(queuedAnimToRemove.name);
                }
            };
            ImGuiHelper::StaticPopupModalManager::Get()->QueueOpenPopop(&animationsEditPopup);
        }
        ImGui::SameLine();
        ImGui::Text("Count: %zu", animatedSpriteComp->animations.size());

        ImGuiHelper::CheckBox ignoreCameraCheckBox("Is Playing", animatedSpriteComp->isPlaying);
        ImGuiHelper::BeginCheckBox(ignoreCameraCheckBox);

        ImGuiHelper::DragFloat2 originDragFloat2("Origin", (float*) &animatedSpriteComp->origin);
        ImGuiHelper::BeginDragFloat2(originDragFloat2);

        ImGuiHelper::ColorEdit4 modulateColorEdit4("Modulate", (float*) &animatedSpriteComp->modulate);
        ImGuiHelper::BeginColorEdit4(modulateColorEdit4);

        ImGuiHelper::CheckBox flipXCheckBox("Flix X", animatedSpriteComp->flipX);
        ImGuiHelper::BeginCheckBox(flipXCheckBox);

        ImGuiHelper::CheckBox flipYCheckBox("Flix Y", animatedSpriteComp->flipY);
        ImGuiHelper::BeginCheckBox(flipYCheckBox);

        ImGui::Separator();
    }
}

void DrawTextLabel(SceneNode* node) {
    if (TextLabelComp* textLabelComp = node->GetComponentSafe<TextLabelComp>()) {
        ImGui::Text("Text Label Component");

        ImGuiHelper::InputText textInputText("Text", textLabelComp->text);
        ImGuiHelper::BeginInputText(textInputText);

        ImGuiHelper::ColorEdit4 colorColorEdit4("Color", (float*) &textLabelComp->color);
        ImGuiHelper::BeginColorEdit4(colorColorEdit4);

        static TextLabelComp* lastTextLabelComp = nullptr;
        static ImGuiHelper::ComboBox fontUIDComboBox("FontUID", { "default", "fight-64" });
        if (textLabelComp != lastTextLabelComp) {
            lastTextLabelComp = textLabelComp;
            fontUIDComboBox.items.clear();
            fontUIDComboBox.items.emplace_back("default");
            ProjectProperties* projectProperties = ProjectProperties::Get();
            for (const auto& fontAsset : projectProperties->assets.fonts) {
                fontUIDComboBox.items.emplace_back(fontAsset.uid);
            }
            fontUIDComboBox.onSelectionChangeCallback = [textLabelComp] (const char* newItem) {
                textLabelComp->fontUID = newItem;
            };
            fontUIDComboBox.SetSelected(textLabelComp->fontUID);
        }
        ImGuiHelper::BeginComboBox(fontUIDComboBox);

        ImGui::Separator();
    }
}

void DrawScript(SceneNode* node) {
    ImGui::Text("Script Component");
    if (ScriptComp* scriptComp = node->GetComponentSafe<ScriptComp>()) {

        ImGuiHelper::InputText classPathInputText("Class Path", scriptComp->classPath);
        ImGuiHelper::BeginInputText(classPathInputText);

        ImGuiHelper::InputText classNameInputText("Class Name", scriptComp->className);
        ImGuiHelper::BeginInputText(classNameInputText);

        if (ImGui::Button("Remove Script")) {
            node->RemoveComponent<ScriptComp>();
        }

        ImGui::Separator();
    } else if (ImGui::Button("Add Script")) {
        node->AddComponent<ScriptComp>();
    }
}

void DrawCollider2D(SceneNode* node) {
    if (Collider2DComp* collider2DComp = node->GetComponentSafe<Collider2DComp>()) {
        ImGui::Text("Collider2D Component");

        ImGuiHelper::DragFloat2 extentsDragFloat2("Extents", (float*) &collider2DComp->extents);
        ImGuiHelper::BeginDragFloat2(extentsDragFloat2);

        ImGuiHelper::ColorEdit4 colorColorEdit4("Color", (float*) &collider2DComp->color);
        ImGuiHelper::BeginColorEdit4(colorColorEdit4);

        ImGui::Separator();
    }
}

void DrawColorRect(SceneNode* node) {
    if (ColorRectComp* colorRectComp = node->GetComponentSafe<ColorRectComp>()) {
        ImGui::Text("Color Rect Component");

        ImGuiHelper::DragFloat2 sizeDragFloat2("Size", (float*) &colorRectComp->size);
        ImGuiHelper::BeginDragFloat2(sizeDragFloat2);

        ImGuiHelper::ColorEdit4 colorColorEdit4("Color", (float*) &colorRectComp->color);
        ImGuiHelper::BeginColorEdit4(colorColorEdit4);

        ImGui::Separator();
    }
}
} // namespace ComponentDetailsDrawUtils

namespace WindowRenderUtils {
EntityArray OnGetSelfAndParentEntitiesFunc(Entity entity) {
    static auto* sceneManager = SceneManager::Get();
    EntityArray combineModelResult = { .entityCount = 0 };
    combineModelResult.entities[combineModelResult.entityCount++] = entity;
    if (auto* node = sceneManager->GetNode(sceneManager->selectedSceneFile, entity)) {
        auto* parent = node->parent;
        while (parent != nullptr) {
            combineModelResult.entities[combineModelResult.entityCount++] = parent->GetUID();
            parent = parent->parent;
        }
    }
    return combineModelResult;
}

Transform2D OnGetLocalTransformFunc(Entity entity, bool* success) {
    static auto* sceneManager = SceneManager::Get();
    if (auto* node = sceneManager->GetNode(sceneManager->selectedSceneFile, entity)) {
        if (auto* transformComp = node->GetComponentSafe<Transform2DComp>()) {
            *success = true;
            return transformComp->transform2D;
        }
    }
    *success = false;
    return {};
}
} // namespace WindowRenderUtils

void OpenedProjectUI::ProcessWindows() {
    int windowWidth = 0;
    int windowHeight = 0;
    SDL_GetWindowSize(editorContext->window, &windowWidth, &windowHeight);

    static ImGuiHelper::Window sceneOutlinerWindow = {
        .name = "Scene Tree",
        .open = nullptr,
        .windowFlags = ImGuiWindowFlags_NoResize,
        .callbackFunc = [] (ImGuiHelper::Context* context) {
            static SceneManager* sceneManager = SceneManager::Get();
            if (ImGui::Button("+")) {
                static ImGuiHelper::PopupModal addNodePopup = {
                    .name = "Add Node Menu",
                    .open = nullptr,
                    .windowFlags = 0,
                    .callbackFunc = [] (ImGuiHelper::Context* context) {
                        if (ImGui::Button("Close")) {
                            ImGui::CloseCurrentPopup();
                        }

                        static std::string selectedType = RBE_NODE_NODE_STRING;
                        static ImGuiHelper::ComboBox nodeTypeSelectionComboBox(
                            "Type",
                        { RBE_NODE_NODE_STRING, RBE_NODE_NODE2D_STRING, RBE_NODE_SPRITE_STRING, RBE_NODE_ANIMATED_SPRITE_STRING, RBE_NODE_TEXT_LABEL_STRING, RBE_NODE_COLLIDER2D_STRING, RBE_NODE_COLOR_RECT_STRING },
                        [](const char* newItem) {
                            selectedType = newItem;
                        }
                        );
                        ImGuiHelper::BeginComboBox(nodeTypeSelectionComboBox);

                        if (ImGui::Button("Add")) {
                            static SceneManager* sceneManager = SceneManager::Get();
                            const NodeBaseType selectedBaseType = node_get_base_type(selectedType.c_str());
                            sceneManager->AddDefaultNodeAsChildToSelected(selectedBaseType);
                            ImGui::CloseCurrentPopup();
                        }
                    },
                    .position = ImVec2{ 100.0f, 100.0f },
                    .size = ImVec2{ 200.0f, 200.0f },
                };
                ImGuiHelper::StaticPopupModalManager::Get()->QueueOpenPopop(&addNodePopup);
            }
            ImGui::SameLine();
            ImGui::Text("%s", sceneManager->selectedSceneFile == nullptr ? "" : sceneManager->selectedSceneFile->filePath.c_str());

            if (auto sceneNodeFile = sceneManager->selectedSceneFile) {
                if (auto rootNode = sceneNodeFile->rootNode) {
                    SceneNodeUtils::DisplayTreeNodeLeaf(rootNode);
                }
            }
        },
        .position = ImVec2{ 150.0f, 100.0f },
        .size = ImVec2{ 400.0f, 300.0f },
    };

    static ImGuiHelper::Window assetImportWindow = {
        .name = "Import",
        .open = nullptr,
        .windowFlags = ImGuiWindowFlags_NoResize,
        .callbackFunc = [] (ImGuiHelper::Context* context) {
            static AssetBrowser* assetBrowser = AssetBrowser::Get();
            static unsigned int selectedIndex = 0;
            if (assetBrowser->selectedFileNode.has_value() && assetBrowser->selectedFileNode->regularFileType != FileNodeRegularFileType::Invalid) {
                const FileNode& selectedFileNode = assetBrowser->selectedFileNode.value();
                ImGui::Text("file: %s", selectedFileNode.path.filename().string().c_str());
                if (selectedFileNode.regularFileType == FileNodeRegularFileType::Texture) {
                    static std::string selectedWrapS = "clamp_to_border";
                    static ImGuiHelper::ComboBox wrapSComboBox("Wrap S", { "clamp_to_border", "repeat" }, [](const char* newItem) {
                        selectedWrapS = newItem;
                    });

                    static std::string selectedWrapT = "clamp_to_border";
                    static ImGuiHelper::ComboBox wrapTComboBox("Wrap T", { "clamp_to_border", "repeat" }, [](const char* newItem) {
                        selectedWrapT = newItem;
                    });

                    static std::string selectedFilterMin = "nearest";
                    static ImGuiHelper::ComboBox filterMinComboBox("Filter Min", { "nearest", "linear" }, [](const char* newItem) {
                        selectedFilterMin = newItem;
                    });

                    static std::string selectedFilterMag = "nearest";
                    static ImGuiHelper::ComboBox filterMagComboBox("Filter Mag", { "nearest", "linear" }, [](const char* newItem) {
                        selectedFilterMag = newItem;
                    });

                    // TODO: Use something else other than index since things can be thrown off if a file is added, deleted, renamed/replaced
                    if (selectedIndex != assetBrowser->selectedFileNode->index) {
                        selectedIndex = assetBrowser->selectedFileNode->index;
                        ProjectProperties* properties = ProjectProperties::Get();
                        auto& textureAsset = properties->GetTextureAsset(selectedFileNode.GetRelativePath());
                        wrapSComboBox.SetSelected(textureAsset.wrap_s);
                        wrapTComboBox.SetSelected(textureAsset.wrap_t);
                        filterMinComboBox.SetSelected(textureAsset.filter_min);
                        filterMagComboBox.SetSelected(textureAsset.filter_mag);
                    }

                    ImGuiHelper::BeginComboBox(wrapSComboBox);
                    ImGuiHelper::BeginComboBox(wrapTComboBox);
                    ImGuiHelper::BeginComboBox(filterMinComboBox);
                    ImGuiHelper::BeginComboBox(filterMagComboBox);

                    ImGui::Separator();
                    if (ImGui::Button("Reimport")) {
                        ProjectProperties* projectProperties = ProjectProperties::Get();
                        const TextureAsset updatedTextureAsset = TextureAsset(selectedFileNode.GetRelativePath(), selectedWrapS, selectedWrapT, selectedFilterMin, selectedFilterMag);
                        projectProperties->UpdateTextureAsset(updatedTextureAsset);
                        ConfigFileCreator::GenerateConfigFile(ProjectProperties::Get());
                    }
                }
                // TODO: Add audio source properties when they exist...
            }
        },
        .position = ImVec2{ 150.0f, 100.0f },
        .size = ImVec2{ 400.0f, 300.0f },
    };

    static ImGuiHelper::Window sceneViewWindow = {
        .name = "Scene View",
        .open = nullptr,
        .windowFlags = ImGuiWindowFlags_NoResize,
        .callbackFunc = [] (ImGuiHelper::Context* context) {
            static Texture* testTexture = rbe_texture_create_solid_colored_texture(1, 1, 255);
            static auto GetNodeTextureRenderTarget = [](SceneNode* node, size_t index, Transform2DComp* transformComp, bool& hasTexture) {
                static AssetManager* assetManager = AssetManager::Get();
                static TransformModel2D globalTransforms[MAX_ENTITIES];
                Texture* renderTargetTexture = testTexture;
                cre_scene_utils_update_global_transform_model(node->GetUID(), &globalTransforms[index]);
                Rect2 sourceRect = { 0.0f, 0.0f, 0.0f, 0.0f };
                Size2D destSize = { 0.0f, 0.0f };
                Color color = { 1.0f, 1.0f, 1.0f, 1.0f };
                bool flipX = false;
                bool flipY = false;
                Vector2 origin = { 0.0f, 0.0f };
                hasTexture = true;
                if (auto* spriteComp = node->GetComponentSafe<SpriteComp>()) {
                    renderTargetTexture = assetManager->GetTexture(spriteComp->texturePath.c_str());
                    sourceRect = spriteComp->drawSource;
                    destSize = { sourceRect.w, sourceRect.h };
                    color = spriteComp->modulate;
                    flipX = spriteComp->flipX;
                    flipY = spriteComp->flipY;
                    origin = spriteComp->origin;
                } else if (auto* animSpriteComp = node->GetComponentSafe<AnimatedSpriteComp>()) {
                    if (!animSpriteComp->currentAnimationName.empty()) {
                        const EditorAnimation& anim = animSpriteComp->GetAnimationByName(animSpriteComp->currentAnimationName);
                        if (!anim.animationFrames.empty()) {
                            // TODO: Get frame from current index instead...
                            const auto& animFrame = anim.animationFrames[0];
                            renderTargetTexture = assetManager->GetTexture(animFrame.texturePath.c_str());
                            sourceRect = animFrame.drawSource;
                            destSize = { sourceRect.w, sourceRect.h };
                            color = animSpriteComp->modulate;
                            flipX = animSpriteComp->flipX;
                            flipY = animSpriteComp->flipY;
                            origin = animSpriteComp->origin;
                        }
                    }
                } else {
                    hasTexture = false;
                }
                cre_scene_utils_apply_camera_and_origin_translation(&globalTransforms[index], &origin, transformComp->ignoreCamera);
                return ImGuiHelper::TextureRenderTarget{
                    .texture = renderTargetTexture,
                    .sourceRect = sourceRect,
                    .destSize = destSize,
                    .color = color,
                    .flipX = flipX,
                    .flipY = flipY,
                    .globalTransform = &globalTransforms[index]
                };
            };
            static SceneManager* sceneManager = SceneManager::Get();
            static AssetManager* assetManager = AssetManager::Get();
            std::vector<ImGuiHelper::TextureRenderTarget> textureRenderTargets;
            std::vector<ImGuiHelper::FontRenderTarget> fontRenderTargets;
            static bool hasBindedSceneUtilsFuncs = false;
            if (!hasBindedSceneUtilsFuncs) {
                cre_scene_utils_override_on_get_local_transform_func(WindowRenderUtils::OnGetLocalTransformFunc);
                cre_scene_utils_override_on_get_self_and_parent_entities_func(WindowRenderUtils::OnGetSelfAndParentEntitiesFunc);
                hasBindedSceneUtilsFuncs = true;
            }
            // Loop through and render all scene nodes starting from the root
            if (sceneManager->selectedSceneFile && sceneManager->selectedSceneFile->rootNode) {
                sceneManager->IterateAllSceneNodes(sceneManager->selectedSceneFile->rootNode, [&textureRenderTargets, &fontRenderTargets](SceneNode* node, size_t i) {
                    if (auto* transformComp = node->GetComponentSafe<Transform2DComp>()) {
                        if (auto* textLabelComp = node->GetComponentSafe<TextLabelComp>()) {
                            TransformModel2D globalTransform = { transformComp->transform2D.position, transformComp->transform2D.scale, transformComp->transform2D.rotation };
                            cre_scene_utils_update_global_transform_model(node->GetUID(), &globalTransform);
                            static Vector2 textLabelOrigin = { 0.0f, 0.0f };
                            cre_scene_utils_apply_camera_and_origin_translation(&globalTransform, &textLabelOrigin, transformComp->ignoreCamera);
                            const ImGuiHelper::FontRenderTarget renderTarget = {
                                .font = assetManager->GetFont(textLabelComp->fontUID.c_str()),
                                .text = textLabelComp->text,
                                .position = globalTransform.position,
                                .scale = globalTransform.scale.x,
                                .color = textLabelComp->color
                            };
                            fontRenderTargets.emplace_back(renderTarget);
                        } else {
                            bool hasTexture = false;
                            const ImGuiHelper::TextureRenderTarget renderTarget = GetNodeTextureRenderTarget(node, i, transformComp, hasTexture);
                            if (hasTexture) {
                                textureRenderTargets.emplace_back(renderTarget);
                            }
                        }
                    }
                });
            }
            ImGuiHelper::WindowRenderer::Render(textureRenderTargets, fontRenderTargets);
        },
        .position = ImVec2{ 300.0f, 100.0f },
        .size = ImVec2{ 400.0f, 300.0f },
    };

    static ImGuiHelper::Window detailsWindow = {
        .name = "Details",
        .open = nullptr,
        .windowFlags = ImGuiWindowFlags_NoResize,
        .callbackFunc = [] (ImGuiHelper::Context* context) {
            static SceneManager* sceneManager = SceneManager::Get();
            if (SceneNode* selectedNode = sceneManager->selectedSceneNode) {
                ImGui::Text("Name: %s", selectedNode->name.c_str());
                ImGui::Text("Type: %s", selectedNode->GetTypeString());
                ImGui::Separator();
                ComponentDetailsDrawUtils::DrawTransform2D(selectedNode);
                ComponentDetailsDrawUtils::DrawSprite(selectedNode);
                ComponentDetailsDrawUtils::DrawAnimatedSprite(selectedNode);
                ComponentDetailsDrawUtils::DrawTextLabel(selectedNode);
                ComponentDetailsDrawUtils::DrawScript(selectedNode);
                ComponentDetailsDrawUtils::DrawCollider2D(selectedNode);
                ComponentDetailsDrawUtils::DrawColorRect(selectedNode);
            }
        },
        .position = ImVec2{ 400.0f, 100.0f },
        .size = ImVec2{ 400.0f, 300.0f },
    };

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
            ImGui::BeginChild("ConsoleLogWindow", ImGui::GetContentRegionAvail(), false, ImGuiWindowFlags_AlwaysHorizontalScrollbar);
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
            static ProcessRunner engineProcess;
            const bool isProcessRunning = engineProcess.IsRunning();
            if (ImGui::Button(">") && !isProcessRunning) {
                if (!ProjectProperties::Get()->initialNodePath.empty()) {
                    if (!engineProcess.Start(editorContext->GetEngineBinaryPath(), editorContext->GetEngineBinaryProgramArgs())) {
                        rbe_logger_error("Failed to start engine process at path '%s'", editorContext->GetEngineBinaryPath().c_str());
                    } else {
                        // Clear console log
                        ConsoleLogger::Get()->Clear();
                    }
                    rbe_logger_debug("Starting engine process at path '%s' with args '%s'",
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
            ImGui::SameLine();
            if (ImGui::Button("[]") && isProcessRunning) {
                engineProcess.Stop();
            }

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
