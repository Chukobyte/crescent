#include "details_ui.h"
#include "../../../../scene/scene_manager.h"
#include "../../../../project_properties.h"
#include "../../../../asset_browser.h"
#include "../../../../editor_callbacks.h"

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
            if (spriteComp->texturePath == ImGuiHelper::COMBO_BOX_LIST_NONE) {
                spriteComp->texturePath.clear();
            }
        };

        static FuncObject initializeFunc = FuncObject([spriteComp] {
            if (spriteComp->texturePath.empty()) {
                spriteTexturePathComboBox.SetSelected(ImGuiHelper::COMBO_BOX_LIST_NONE);
            } else {
                spriteTexturePathComboBox.SetSelected(spriteComp->texturePath);
            }
            EditorCallbacks::Get()->RegisterOnSceneNodeSelected([](SceneNode* sceneNode) {
                spriteTexturePathComboBox.onSelectionChangeCallback = nullptr; // Disable on selection call back while switching nodes to prevent previous node from being set
                if (auto spriteC = sceneNode->GetComponentSafe<SpriteComp>()) {
                    if (spriteC->texturePath.empty()) {
                        spriteTexturePathComboBox.SetSelected(ImGuiHelper::COMBO_BOX_LIST_NONE);
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

        static std::vector<std::string> spriteAnimationList = {ImGuiHelper::COMBO_BOX_LIST_NONE };
        static auto UpdateSpriteAnimationList = [] (AnimatedSpriteComp* animSpriteComp) {
            spriteAnimationList.clear();
            spriteAnimationList.emplace_back(ImGuiHelper::COMBO_BOX_LIST_NONE);
            for (const auto& anim : animSpriteComp->animations) {
                spriteAnimationList.emplace_back(anim.name);
            }
        };
        UpdateSpriteAnimationList(animatedSpriteComp);
        static ImGuiHelper::ComboBox spriteAnimationSelectionComboBox("Current Animation", spriteAnimationList);
        spriteAnimationSelectionComboBox.items = spriteAnimationList;
        spriteAnimationSelectionComboBox.onSelectionChangeCallback = [animatedSpriteComp] (const char* newItem) {
            animatedSpriteComp->currentAnimationName = newItem;
            if (animatedSpriteComp->currentAnimationName == ImGuiHelper::COMBO_BOX_LIST_NONE) {
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
                    static std::vector<std::string> animFrameTexturePathList = {ImGuiHelper::COMBO_BOX_LIST_NONE };
                    static AssetBrowser* assetBrowser = AssetBrowser::Get();
                    static auto UpdateTexturePathList = [] {
                        animFrameTexturePathList.clear();
                        animFrameTexturePathList.emplace_back(ImGuiHelper::COMBO_BOX_LIST_NONE);
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
                        animFrameTexturePathComboBox.SetSelected(ImGuiHelper::COMBO_BOX_LIST_NONE, false);
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
                                animFrameTexturePathComboBox.SetSelected(ImGuiHelper::COMBO_BOX_LIST_NONE);
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
                                    if (animFrame.texturePath == ImGuiHelper::COMBO_BOX_LIST_NONE) {
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
                            const std::string newSelectedAnimFrameTexturePath = !newSelectedAnimFrame.texturePath.empty() ? newSelectedAnimFrame.texturePath : ImGuiHelper::COMBO_BOX_LIST_NONE;
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

ImGuiHelper::Window OpenedProjectUI::Windows::GetDetailsWindow() {
    ImGuiHelper::Window detailsWindow = {
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
    return detailsWindow;
}
