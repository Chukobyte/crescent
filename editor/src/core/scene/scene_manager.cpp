#include "scene_manager.h"

#include <IconsFontAwesome6.h>

#include "../seika/utils/se_assert.h"

#include "../engine/src/core/json/json_file_loader.h"
#include "../engine/src/core/game_properties.h"

#include "../ui/imgui/imgui_helper.h"
#include "../ui/imgui/imgui_file_browser.h"
#include "../editor_callbacks.h"
#include "../project_properties.h"

//--- Scene Node Utils ---//
// TODO: Fix issue with not registering 'Right Click' logic because tree node is closed...
void SceneNodeUtils::DisplayTreeNodeLeaf(SceneNode *sceneNode) {
    static SceneManager* sceneManager = SceneManager::Get();
    static ImGuiTreeNodeFlags defaultFlags = ImGuiTreeNodeFlags_DefaultOpen;
    static EditorCallbacks* editorCallbacks = EditorCallbacks::Get();
    const ImGuiTreeNodeFlags currentFlags = sceneNode->children.empty() || sceneNode->IsExternalSceneNode() ? defaultFlags | ImGuiTreeNodeFlags_Leaf : defaultFlags;
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
                // TODO: Move somewhere else (like make it a button on top of the scene outliner instead)
                static ImGuiHelper::FileBrowser externalNodeFileBrowser = {
                    .name = "External Scene Browser",
                    .open = nullptr,
                    .windowFlags = ImGuiWindowFlags_NoResize,
                    .callbackFunc = nullptr,
                    .position = ImVec2{ 100.0f, 100.0f },
                    .size = ImVec2{ 600.0f, 320.0f },
                    .rootPath = {},
                    .mode = ImGuiHelper::FileBrowser::Mode::OpenFile,
                    .validExtensions = { ".cscn" }
                };
                if (sceneNode->IsExternalSceneNode() && ImGui::MenuItem("Make external scene local")) {
                    sceneNode->externalNodeSource.clear();
                } else if (!sceneNode->IsExternalSceneNode() && ImGui::MenuItem("Add external scene")) {
                    ImGuiHelper::FileBrowserPopupManager::Get()->QueueOpenPopop(&externalNodeFileBrowser);
                }

                static ProjectProperties* projectProperties = ProjectProperties::Get();
                externalNodeFileBrowser.rootPath = projectProperties->projectPath;
                externalNodeFileBrowser.onModeCompletedFunc = [sceneNode](const std::filesystem::path& fullPath) {
                    const std::string fullPathString = fullPath.generic_string();
                    const std::string relativePath = projectProperties->GetPathRelativeToProjectPath(fullPathString);
                    // Make sure we aren't adding current scene as external scene
                    if (relativePath != sceneManager->selectedSceneFile->filePath) {
                        SceneNodeFile* externalSceneFile = sceneManager->LoadSceneFromFile(fullPathString.c_str());
                        externalSceneFile->rootNode->parent = sceneNode;
                        externalSceneFile->rootNode->externalNodeSource = relativePath;
                        sceneManager->IterateAllSceneNodes(externalSceneFile->rootNode, [](SceneNode* node, size_t index) {
                            node->doesOriginateFromExternalScene = true;
                        });
                        sceneNode->children.emplace_back(externalSceneFile->rootNode);
                    }
                };

                if (sceneNode->parent != nullptr && ImGui::MenuItem("Delete")) {
                    // TODO: Delete Node
                    sceneManager->QueueNodeForDeletion(sceneNode);
                }
                ImGui::EndPopup();
            }

            for (auto* sceneNodeChild : sceneNode->children) {
                // Don't display children external scenes for now
                if (!sceneNodeChild->AreParentsInExternalScene()) {
                    DisplayTreeNodeLeaf(sceneNodeChild);
                }
            }
        }
    };
    ImGuiHelper::BeginTreeNode(treeNode);
    if (sceneNode->IsExternalSceneNode()) {
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3.0f);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 1.0f);
        ImGui::Text(ICON_FA_LINK);
    }
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
    SE_ASSERT_FMT(rootSceneNode->parent == nullptr, "Scene tree root has a parent!?");
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
    if ((NodeBaseInheritanceType_PARALLAX & inheritanceType) == NodeBaseInheritanceType_PARALLAX) {
        newNode->AddComponent<ParallaxComp>();
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

SceneNodeFile* SceneManager::GenerateSceneNodeFileFromJson(JsonSceneNode* rootTreeNode, const char* sceneFilePath) {
    SE_ASSERT_FMT(rootTreeNode->parent == nullptr, "Scene tree root has a parent!?  At path '%s'", sceneFilePath);
    auto* nodeFile = new SceneNodeFile{ sceneFilePath };
    nodeFile->rootNode = LoadSceneTreeJson(rootTreeNode);
    return nodeFile;
}

SceneNode* SceneManager::LoadSceneTreeJson(JsonSceneNode* node, SceneNode* parent) {
    auto* sceneNode = new SceneNode();
    sceneNode->parent = parent;
    sceneNode->name = node->name;
    sceneNode->type = node->type;
    if (node->externalNodeSource != nullptr) {
        sceneNode->externalNodeSource = std::string(node->externalNodeSource);
    }
    sceneNode->doesOriginateFromExternalScene = node->fromExternalNodeSource;

    // Shader Paths
    const std::string shaderPath = node->shaderInstanceShaderPath != nullptr ? std::string(node->shaderInstanceShaderPath) : "";

    // Components
    if (node->components[CreComponentDataIndex_TRANSFORM_2D] != nullptr) {
        sceneNode->AddComponent<Transform2DComp>((Transform2DComponent*) node->components[CreComponentDataIndex_TRANSFORM_2D]);
    }
    if (node->components[CreComponentDataIndex_SPRITE] != nullptr) {
        sceneNode->AddComponent<SpriteComp>((SpriteComponent*) node->components[CreComponentDataIndex_SPRITE], node->spriteTexturePath, shaderPath);
    }
    if (node->components[CreComponentDataIndex_ANIMATED_SPRITE] != nullptr) {
        sceneNode->AddComponent<AnimatedSpriteComp>((AnimatedSpriteComponentData*) node->components[CreComponentDataIndex_ANIMATED_SPRITE], shaderPath);
    }
    if (node->components[CreComponentDataIndex_TEXT_LABEL] != nullptr) {
        std::string nodeFontUID = std::string(node->fontUID);
        if (nodeFontUID == CRE_DEFAULT_FONT_ASSET.uid) {
            nodeFontUID.clear();
        }
        sceneNode->AddComponent<TextLabelComp>((TextLabelComponent*) node->components[CreComponentDataIndex_TEXT_LABEL], nodeFontUID);
    }
    if (node->components[CreComponentDataIndex_SCRIPT] != nullptr) {
        sceneNode->AddComponent<ScriptComp>((ScriptComponent*) node->components[CreComponentDataIndex_SCRIPT]);
    }
    if (node->components[CreComponentDataIndex_COLLIDER_2D] != nullptr) {
        sceneNode->AddComponent<Collider2DComp>((Collider2DComponent*) node->components[CreComponentDataIndex_COLLIDER_2D]);
    }
    if (node->components[CreComponentDataIndex_COLOR_RECT] != nullptr) {
        sceneNode->AddComponent<ColorRectComp>((ColorRectComponent*) node->components[CreComponentDataIndex_COLOR_RECT]);
    }
    if (node->components[CreComponentDataIndex_PARALLAX] != nullptr) {
        sceneNode->AddComponent<ParallaxComp>((ParallaxComponent*) node->components[CreComponentDataIndex_PARALLAX]);
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

SceneNode* SceneManager::GetNode(SceneNodeFile* nodeFile, CreEntity entity) {
    return GetNode(nodeFile->rootNode, entity);
}

SceneNode* SceneManager::GetNode(SceneNode* node, CreEntity entity) {
    if (node->GetUID() == entity) {
        return node;
    }
    for (auto* childNode : node->children) {
        if (auto* foundChildNode = GetNode(childNode, entity)) {
            return foundChildNode;
        }
    }
    return nullptr;
}
