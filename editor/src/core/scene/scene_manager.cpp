#include "scene_manager.h"

#include "../engine/src/core/scripting/python/py_helper.h"
#include "../engine/src/core/json/json_file_loader.h"
#include "../engine/src/core/utils/logger.h"
#include "../engine/src/core/utils/rbe_assert.h"
#include "../ui/imgui/imgui_helper.h"
#include "../editor_callbacks.h"

//--- Scene Node Utils ---//
// TODO: Fix issue with not registering 'Right Click' logic because tree node is closed...
void SceneNodeUtils::DisplayTreeNodeLeaf(SceneNode *sceneNode) {
    static SceneManager* sceneManager = SceneManager::Get();
    static ImGuiTreeNodeFlags defaultFlags = ImGuiTreeNodeFlags_DefaultOpen;
    static EditorCallbacks* editorCallbacks = EditorCallbacks::Get();
    const ImGuiTreeNodeFlags currentFlags = sceneNode->children.empty() ? defaultFlags | ImGuiTreeNodeFlags_Leaf : defaultFlags;
    ImGuiHelper::TreeNode treeNode = {
        .label = sceneNode->name,
        .flags = sceneManager->selectedSceneNode == sceneNode ? ImGuiTreeNodeFlags_Selected | currentFlags : currentFlags,
        .callbackFunc = [sceneNode] (ImGuiHelper::Context* context) {
            // Left Click
            if (ImGui::IsItemClicked()) {
                sceneManager->selectedSceneNode = sceneNode;
                editorCallbacks->BroadcastOnSceneNodeSelected(sceneManager->selectedSceneNode);
            }

            // Right Click
            const std::string nodePopupId = sceneNode->name + "_popup" + std::to_string(sceneNode->GetUID());
            ImGui::OpenPopupOnItemClick(nodePopupId.c_str(), ImGuiPopupFlags_MouseButtonRight);
            if (ImGui::BeginPopup(nodePopupId.c_str())) {
                sceneManager->selectedSceneNode = sceneNode;
                editorCallbacks->BroadcastOnSceneNodeSelected(sceneManager->selectedSceneNode);
                if (ImGui::MenuItem("Rename")) {
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
                    ImGuiHelper::StaticPopupModalManager::Get()->QueueOpenPopop(&renameNodePopup);
                }
                if (sceneNode->parent != nullptr && ImGui::MenuItem("Delete")) {
                    // TODO: Delete Node
                    sceneManager->QueueNodeForDeletion(sceneNode);
                }
                ImGui::EndPopup();
            }

            for (auto* sceneNodeChild : sceneNode->children) {
                DisplayTreeNodeLeaf(sceneNodeChild);
            }
        }
    };
    ImGuiHelper::BeginTreeNode(treeNode);
}

//--- Scene Manager ---//
SceneNodeFile* SceneManager::LoadSceneFromFile(const char* sceneFilePath, bool forceLoadFromDisk) {
    if (!forceLoadFromDisk && loadedSceneFiles.count(sceneFilePath) > 0) {
        return loadedSceneFiles[sceneFilePath];
    }
    if (JsonSceneNode* rootJsonSceneNode = cre_json_load_scene_file(sceneFilePath)) {
        SceneNodeFile* nodeFile = GenerateSceneNodeFileFromJson(rootJsonSceneNode, sceneFilePath);
        nodeFile->hasBeenSaved = true;
        loadedSceneFiles.emplace(sceneFilePath, nodeFile);
        if (selectedSceneFile == nullptr) {
            selectedSceneFile = nodeFile;
        }
        if (selectedSceneNode == nullptr) {
            selectedSceneNode = nodeFile->rootNode;
        }
        return nodeFile;
    }
    return nullptr;
}

SceneNodeFile *SceneManager::GenerateDefaultSceneNodeFile() const {
    SceneNodeFile* nodeFile = new SceneNodeFile();
    nodeFile->filePath = "[unsaved]";
    return nodeFile;
}

SceneNodeFile* SceneManager::GenerateDefaultSceneNodeFile(SceneNode* rootSceneNode) const {
    RBE_ASSERT_FMT(rootSceneNode->parent == nullptr, "Scene tree root has a parent!?");
    SceneNodeFile* nodeFile = new SceneNodeFile();
    nodeFile->rootNode = rootSceneNode;
    return nodeFile;
}

void SceneManager::ResetCurrentSceneNodeFile() {
    if (selectedSceneFile) {
        QueueNodeForDeletion(selectedSceneFile->rootNode);
        selectedSceneFile->rootNode = nullptr;
        selectedSceneFile->hasBeenSaved = false;
        selectedSceneFile->filePath.clear();
    }
}

void SceneManager::AddDefaultNodeAsChildToSelected(NodeBaseType type) {
    SceneNode* newNode = new SceneNode();
    newNode->name = GetUniqueNodeName(node_get_base_type_string(type), selectedSceneNode);
    newNode->type = type;
    // Establish parent and child relationship
    newNode->parent = selectedSceneNode;
    if (selectedSceneNode != nullptr) {
        selectedSceneNode->children.emplace_back(newNode);
    }
    // Setup components based on type
    const NodeBaseInheritanceType inheritanceType = node_get_type_inheritance(type);
    if ((NodeBaseInheritanceType_NODE2D & inheritanceType) == NodeBaseInheritanceType_NODE2D) {
        newNode->AddComponent<Transform2DComp>();
    }
    if ((NodeBaseInheritanceType_SPRITE & inheritanceType) == NodeBaseInheritanceType_SPRITE) {
        newNode->AddComponent<SpriteComp>();
    }
    if ((NodeBaseInheritanceType_ANIMATED_SPRITE & inheritanceType) == NodeBaseInheritanceType_ANIMATED_SPRITE) {
        newNode->AddComponent<AnimatedSpriteComp>();
    }
    if ((NodeBaseInheritanceType_TEXT_LABEL & inheritanceType) == NodeBaseInheritanceType_TEXT_LABEL) {
        newNode->AddComponent<TextLabelComp>();
    }
    if ((NodeBaseInheritanceType_COLLIDER2D & inheritanceType) == NodeBaseInheritanceType_COLLIDER2D) {
        newNode->AddComponent<Collider2DComp>();
    }
    if ((NodeBaseInheritanceType_COLOR_RECT & inheritanceType) == NodeBaseInheritanceType_COLOR_RECT) {
        newNode->AddComponent<ColorRectComp>();
    }

    if (selectedSceneFile->rootNode == nullptr) {
        selectedSceneFile->rootNode = newNode;
    }
}

void SceneManager::QueueNodeForDeletion(SceneNode* nodeToDelete, bool recurseChildren) {
    if (recurseChildren) {
        for (auto* childToDelete : nodeToDelete->children) {
            QueueNodeForDeletion(childToDelete, true);
        }
    }
    nodesQueuedForDeletion.emplace_back(nodeToDelete);
}

void SceneManager::FlushQueuedForDeletionNodes() {
    for (auto nodeToDelete : nodesQueuedForDeletion) {
        // TODO: Use smart pointers...
        if (nodeToDelete == selectedSceneNode) {
            selectedSceneNode = nullptr;
        }
        if (SceneNode* parent = nodeToDelete->parent) {
            parent->RemoveChild(nodeToDelete);
        }
    }
    nodesQueuedForDeletion.clear();
}

std::string SceneManager::GetUniqueNodeName(const std::string& nameCandidate, SceneNode *parent) {
    if (parent == nullptr) {
        return nameCandidate;
    }
    std::string currentCandidate = nameCandidate;
    bool hasDuplicateName = false;
    // Initial check
    for (const auto& node : parent->children) {
        if (node->name == currentCandidate) {
            hasDuplicateName = true;
        }
    }

    if (hasDuplicateName) {
        size_t endDigitIndex = nameCandidate.find_last_not_of("0123456789");
        std::string nameWithoutDigits = nameCandidate.substr(0, endDigitIndex + 1);
        std::string endDigits = nameCandidate.substr(endDigitIndex + 1);
        int currentIndex = 0;
        currentCandidate = nameWithoutDigits + std::to_string(currentIndex);

        // TODO: Make a more efficient algo
        while (true) {
            hasDuplicateName = false;
            for (const auto& node : parent->children) {
                if (node->name == currentCandidate) {
                    currentCandidate = nameWithoutDigits + std::to_string(currentIndex);
                    hasDuplicateName = true;
                    break;
                }
            }
            if (!hasDuplicateName) {
                break;
            }
            currentIndex++;
        }
    }

    return currentCandidate;
}

SceneNodeFile* SceneManager::GenerateSceneNodeFileFromJson(JsonSceneNode *rootTreeNode, const char *sceneFilePath) {
    RBE_ASSERT_FMT(rootTreeNode->parent == nullptr, "Scene tree root has a parent!?  At path '%s'", sceneFilePath);
    SceneNodeFile* nodeFile = new SceneNodeFile{ sceneFilePath };
    nodeFile->rootNode = LoadSceneTreeJson(rootTreeNode);
    return nodeFile;
}

SceneNode* SceneManager::LoadSceneTreeJson(JsonSceneNode* node, SceneNode* parent) {
    SceneNode* sceneNode = new SceneNode();
    sceneNode->parent = parent;
    sceneNode->name = node->name;
    sceneNode->type = node->type;

    // Components
    if (node->components[ComponentDataIndex_TRANSFORM_2D] != nullptr) {
        Transform2DComponent* transform2DComponent = transform2d_component_copy((Transform2DComponent*) node->components[ComponentDataIndex_TRANSFORM_2D]);
        sceneNode->AddComponent<Transform2DComp>(transform2DComponent);
    }
    if (node->components[ComponentDataIndex_SPRITE] != nullptr) {
        SpriteComponent* spriteComponent = sprite_component_copy((SpriteComponent*) node->components[ComponentDataIndex_SPRITE]);
        sceneNode->AddComponent<SpriteComp>(spriteComponent, node->spriteTexturePath);
    }
    if (node->components[ComponentDataIndex_ANIMATED_SPRITE] != nullptr) {
        AnimatedSpriteComponentData* animatedSpriteComponentData = (AnimatedSpriteComponentData*) node->components[ComponentDataIndex_ANIMATED_SPRITE];
        sceneNode->AddComponent<AnimatedSpriteComp>(animatedSpriteComponentData);
    }
    if (node->components[ComponentDataIndex_TEXT_LABEL] != nullptr) {
        TextLabelComponent* textLabelComponent = text_label_component_copy((TextLabelComponent*) node->components[ComponentDataIndex_TEXT_LABEL]);
        sceneNode->AddComponent<TextLabelComp>(textLabelComponent, node->fontUID);
    }
    if (node->components[ComponentDataIndex_SCRIPT] != nullptr) {
        ScriptComponent* scriptComponent = script_component_copy((ScriptComponent*) node->components[ComponentDataIndex_SCRIPT]);
        sceneNode->AddComponent<ScriptComp>(scriptComponent);
    }
    if (node->components[ComponentDataIndex_COLLIDER_2D] != nullptr) {
        Collider2DComponent* collider2DComponent = collider2d_component_copy((Collider2DComponent*) node->components[ComponentDataIndex_COLLIDER_2D]);
        sceneNode->AddComponent<Collider2DComp>(collider2DComponent);
    }
    if (node->components[ComponentDataIndex_COLOR_RECT] != nullptr) {
        ColorRectComponent* colorSquareComponent = color_rect_component_copy(
                    (ColorRectComponent *) node->components[ComponentDataIndex_COLOR_RECT]);
        sceneNode->AddComponent<ColorRectComp>(colorSquareComponent);
    }

    // Load children
    for (size_t i = 0; i < node->childrenCount; i++) {
        SceneNode* childSceneNode = LoadSceneTreeJson(node->children[i], sceneNode);
        sceneNode->children.emplace_back(childSceneNode);
    }
    return sceneNode;
}

namespace {
void LoopNodes(SceneNode* node, std::function<void(SceneNode*, size_t)>& func, size_t& index) {
    func(node, index);
    index++;
    for (auto* child : node->children) {
        LoopNodes(child, func, index);
    }
}
}

void SceneManager::IterateAllSceneNodes(SceneNode* node, std::function<void(SceneNode*, size_t)> func) {
    size_t index = 0;
    LoopNodes(node, func, index);
}
