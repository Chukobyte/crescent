#include "opened_project_ui.h"

#include "../engine/src/core/utils/rbe_file_system_utils.h"
#include "../engine/src/core/utils/logger.h"

#include "imgui/imgui_helper.h"
#include "../editor_context.h"
#include "../project_properties.h"
#include "../utils/helper.h"
#include "../file_creation/config_file_creator.h"
#include "../scene/scene_manager.h"
#include "../asset_browser.h"

const char* CONFIG_FILE_NAME = "test_cre_config.py";

static EditorContext* editorContext = EditorContext::Get();

void OpenedProjectUI::ProcessMenuBar() {
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
                        },
                    },
                    {
                        .name = "Open Scene",
                        .keyboardShortcut = "Ctrl+N",
                        .callbackFunc = [] (ImGuiHelper::Context* context) {
                        },
                    },
                    {
                        .name = "Go To Project Manager",
                        .keyboardShortcut = "Ctrl+Shift+Q",
                        .callbackFunc = [] (ImGuiHelper::Context* context) {
                            context->OpenPopup("Confirm Go To Project Manager");
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
    static ImGuiHelper::Context* imguiHelperContext = ImGuiHelper::Context::Get();
    imguiHelperContext->FlushPopups();

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

    static ImGuiHelper::PopupModal inputConfigurationPopup = {
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
    ImGuiHelper::BeginPopupModal(inputConfigurationPopup);

    static ImGuiHelper::PopupModal fontConfigurationPopup = {
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
                    fontIndexToDelete = (int) i;
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
    ImGuiHelper::BeginPopupModal(fontConfigurationPopup);

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
                // TODO: Implement
                static SceneManager* sceneManager = SceneManager::Get();
                const NodeBaseType selectedBaseType = node_get_base_type(selectedType.c_str());
                sceneManager->AddDefaultNodeAsChildToSelected(selectedBaseType);
                ImGui::CloseCurrentPopup();
            }
        },
        .position = ImVec2{ 100.0f, 100.0f },
        .size = ImVec2{ 200.0f, 200.0f },
    };
    ImGuiHelper::BeginPopupModal(addNodePopup);

    static ImGuiHelper::PopupModal renameNodePopup = {
        .name = "Rename Node Menu",
        .open = nullptr,
        .windowFlags = 0,
        .callbackFunc = [] (ImGuiHelper::Context* context) {

            static std::string newNameText;
            ImGuiHelper::InputText newNameInputText("New Name", newNameText);
            ImGuiHelper::BeginInputText(newNameInputText);

            if (ImGui::Button("Close")) {
                newNameText.clear();
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Ok")) {
                static SceneManager* sceneManager = SceneManager::Get();
                if (!newNameText.empty() && sceneManager->selectedSceneNode != nullptr) {
                    if (sceneManager->selectedSceneNode->parent != nullptr) {
                        newNameText = SceneManager::GetUniqueNodeName(newNameText, sceneManager->selectedSceneNode->parent);
                    }
                    sceneManager->selectedSceneNode->name = newNameText;
                }
                newNameText.clear();
                ImGui::CloseCurrentPopup();
            }
        },
        .position = ImVec2{ 100.0f, 100.0f },
        .size = ImVec2{ 200.0f, 200.0f },
    };
    ImGuiHelper::BeginPopupModal(renameNodePopup);
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

void DrawSprite(SceneNode* node) {
    if (SpriteComp* spriteComp = node->GetComponentSafe<SpriteComp>()) {
        ImGui::Text("Sprite Component");

        // TODO: Configured textures should have there paths loaded into a combo box
        ImGui::Text("Texture Path: %s", spriteComp->texturePath.c_str());

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

        // TODO: Add animations panel

        ImGui::Text("Current Animation: %s", animatedSpriteComp->currentAnimationName.c_str());

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
    if (ScriptComp* scriptComp = node->GetComponentSafe<ScriptComp>()) {
        ImGui::Text("Script Component");

        ImGuiHelper::InputText classPathInputText("Class Path", scriptComp->classPath);
        ImGuiHelper::BeginInputText(classPathInputText);

        ImGuiHelper::InputText classNameInputText("Class Name", scriptComp->className);
        ImGuiHelper::BeginInputText(classNameInputText);

        ImGui::Separator();
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

void OpenedProjectUI::ProcessWindows() {
    int windowWidth = 0;
    int windowHeight = 0;
    SDL_GetWindowSize(editorContext->window, &windowWidth, &windowHeight);

    static ImGuiHelper::Window sceneOutlinerWindow = {
        .name = "Scene Tree",
        .open = nullptr,
        .windowFlags = ImGuiWindowFlags_NoResize,
        .callbackFunc = [] (ImGuiHelper::Context* context) {
            if (ImGui::Button("+")) {
                context->OpenPopup("Add Node Menu");
            }

            static SceneManager* sceneManager = SceneManager::Get();
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
                        ConfigFileCreator::GenerateConfigFile(CONFIG_FILE_NAME, ProjectProperties::Get());
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
        .callbackFunc = [] (ImGuiHelper::Context* context) {},
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
            FileNodeUtils::DisplayFileNodeTree(assetBrowser->rootNode, true);
        },
        .position = ImVec2{ 100.0f, 200.0f },
        .size = ImVec2{ 400.0f, 300.0f },
    };

    static ImGuiHelper::Window consoleWindow = {
        .name = "Console",
        .open = nullptr,
        .windowFlags = ImGuiWindowFlags_NoResize,
        .callbackFunc = [] (ImGuiHelper::Context* context) {},
        .position = ImVec2{ 200.0f, 200.0f },
        .size = ImVec2{ 400.0f, 300.0f },
    };

    static ImGuiHelper::DockSpace dockSpace = {
        .id = "DockSpace",
        .size = ImVec2((float) windowWidth, (float) windowHeight),
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
