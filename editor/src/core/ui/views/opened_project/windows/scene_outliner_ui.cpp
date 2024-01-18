#include "scene_outliner_ui.h"
#include "../../../../scene/scene_manager.h"

ImGuiHelper::Window OpenedProjectUI::Windows::GetSceneOutlinerWindow() {
    ImGuiHelper::Window sceneOutlinerWindow = {
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

                        static std::string selectedType = CRE_NODE_NODE_STRING;
                        static ImGuiHelper::ComboBox nodeTypeSelectionComboBox(
                            "Type",
                        {CRE_NODE_NODE_STRING, CRE_NODE_NODE2D_STRING, CRE_NODE_SPRITE_STRING, CRE_NODE_ANIMATED_SPRITE_STRING, CRE_NODE_TEXT_LABEL_STRING, CRE_NODE_COLLIDER2D_STRING, CRE_NODE_COLOR_RECT_STRING, CRE_NODE_PARALLAX_STRING, CRE_NODE_PARTICLES2D_STRING },
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
    return sceneOutlinerWindow;
}
