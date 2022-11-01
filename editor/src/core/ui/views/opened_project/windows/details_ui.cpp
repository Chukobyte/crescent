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
            static OpenedProjectUI::Windows::AnimationEditor* animationEditor = OpenedProjectUI::Windows::AnimationEditor::Get();
            ImGuiHelper::StaticPopupModalManager::Get()->QueueOpenPopop(&animationEditor->GetPopup(animatedSpriteComp));
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
