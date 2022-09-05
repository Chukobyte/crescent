#include "scene_manager.h"

#include "../engine/src/core/scripting/python/py_helper.h"
#include "../engine/src/core/utils/logger.h"
#include "../engine/src/core/utils/rbe_assert.h"
#include "../ui/imgui/imgui_helper.h"

//--- Scene Node Utils ---//
void SceneNodeUtils::DisplayTreeNodeLeaf(SceneNode *sceneNode) {
    static SceneManager* sceneManager = SceneManager::Get();
    static ImGuiTreeNodeFlags defaultFlags = ImGuiTreeNodeFlags_DefaultOpen;
    ImGuiHelper::TreeNode treeNode = {
        .label = sceneNode->name,
        .flags = sceneManager->selectedSceneNode == sceneNode ? ImGuiTreeNodeFlags_Selected | defaultFlags : defaultFlags,
        .callbackFunc = [sceneNode] (ImGuiHelper::Context* context) {
            // Left Click
            if (ImGui::IsItemClicked()) {
                sceneManager->selectedSceneNode = sceneNode;
            }

            // Right Click
            const std::string nodePopupId = sceneNode->name + "_popup" + std::to_string(sceneNode->GetUID());
            ImGui::OpenPopupOnItemClick(nodePopupId.c_str(), ImGuiPopupFlags_MouseButtonRight);
            if (ImGui::BeginPopup(nodePopupId.c_str())) {
                if (ImGui::MenuItem("Rename")) {

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
bool SceneManager::LoadSceneFromFile(const char *sceneFilePath) {
    pyh_run_python_file(sceneFilePath);
    if (FileSceneNode* rootFileSceneNode = file_scene_node_get_cached_file_scene_node()) {
        SceneNodeFile* nodeFile = GenerateSceneNodeFile(rootFileSceneNode, sceneFilePath);
        loadedSceneFiles.emplace_back(nodeFile);
        if (selectedSceneFile == nullptr) {
            selectedSceneFile = nodeFile;
        }
        if (selectedSceneNode == nullptr) {
            selectedSceneNode = nodeFile->rootNode;
        }
        file_scene_node_delete_cached_file_scene_node();
        return true;
    }
    return false;
}

SceneNodeFile *SceneManager::GenerateSceneNodeFile(FileSceneNode *rootTreeNode, const char* sceneFilePath) {
    RBE_ASSERT_FMT(rootTreeNode->parent == nullptr, "Scene tree root has a parent!?  At path '%s'", sceneFilePath);
    SceneNodeFile* nodeFile = new SceneNodeFile{ sceneFilePath };
    nodeFile->rootNode = LoadSceneTreeNode(rootTreeNode);
    return nodeFile;
}

SceneNode* SceneManager::LoadSceneTreeNode(FileSceneNode* node, SceneNode* parent) {
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
        SceneNode* childSceneNode = LoadSceneTreeNode(node->children[i], sceneNode);
        sceneNode->children.emplace_back(childSceneNode);
    }
    return sceneNode;
}

void SceneManager::AddDefaultNodeAsChildToSelected(NodeBaseType type) {
    SceneNode* newNode = new SceneNode();
    newNode->name = GetUniqueNodeName(node_get_base_type_string(type), selectedSceneNode);
    newNode->type = type;
    if (selectedSceneNode) {
        // Establish parent and child relationship
        newNode->parent = selectedSceneNode;
        selectedSceneNode->children.emplace_back(newNode);
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
    } else {
        // TODO: Create new node for newly created scenes
    }
}

void SceneManager::QueueNodeForDeletion(SceneNode* nodeToDelete) {
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
        delete nodeToDelete;
    }
    nodesQueuedForDeletion.clear();
}

std::string SceneManager::GetUniqueNodeName(const std::string& nameCandidate, SceneNode *parent) {
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
