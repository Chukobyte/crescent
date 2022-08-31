#include "scene_manager.h"

#include "../engine/src/core/scripting/python/py_helper.h"
#include "../engine/src/core/utils/logger.h"
#include "../engine/src/core/utils/rbe_assert.h"

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
    if (node->components[ComponentDataIndex_COLOR_SQUARE] != nullptr) {
        ColorSquareComponent* colorSquareComponent = color_square_component_copy((ColorSquareComponent*) node->components[ComponentDataIndex_COLOR_SQUARE]);
        sceneNode->AddComponent<ColorSquareComp>(colorSquareComponent);
    }

    // Load children
    for (size_t i = 0; i < node->childrenCount; i++) {
        SceneNode* childSceneNode = LoadSceneTreeNode(node->children[i], sceneNode);
        sceneNode->children.emplace_back(childSceneNode);
    }
    return sceneNode;
}
