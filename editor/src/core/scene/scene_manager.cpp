#include "scene_manager.h"

#include "../engine/src/core/scripting/python/py_helper.h"
#include "../engine/src/core/ecs/component/transform2d_component.h"
#include "../engine/src/core/ecs/component/sprite_component.h"
#include "../engine/src/core/ecs/component/animated_sprite_component.h"
#include "../engine/src/core/ecs/component/text_label_component.h"
#include "../engine/src/core/ecs/component/script_component.h"
#include "../engine/src/core/ecs/component/collider2d_component.h"
#include "../engine/src/core/ecs/component/color_square_component.h"
#include "../engine/src/core/utils/logger.h"
#include "../engine/src/core/utils/rbe_assert.h"

bool SceneManager::LoadSceneFromFile(const char *sceneFilePath) {
    pyh_run_python_file(sceneFilePath);
    if (FileSceneNode* rootFileSceneNode = file_scene_node_get_cached_file_scene_node()) {
        SceneNodeFile* nodeFile = GenerateSceneNodeFile(rootFileSceneNode, sceneFilePath);
        loadedSceneFiles.emplace_back(nodeFile);
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

    // Component
    for (size_t i = 0; i < MAX_COMPONENTS; i++) {
        sceneNode->components[i] = nullptr;
    }

    if (node->components[ComponentDataIndex_TRANSFORM_2D] != nullptr) {
        Transform2DComponent* transform2DComponent = transform2d_component_copy((Transform2DComponent*) node->components[ComponentDataIndex_TRANSFORM_2D]);
        sceneNode->components[ComponentDataIndex_TRANSFORM_2D] = transform2DComponent;
    }
    if (node->components[ComponentDataIndex_SPRITE] != nullptr) {
        SpriteComponent* spriteComponent = sprite_component_copy((SpriteComponent*) node->components[ComponentDataIndex_SPRITE]);
        sceneNode->components[ComponentDataIndex_SPRITE] = spriteComponent;
    }
    if (node->components[ComponentDataIndex_ANIMATED_SPRITE] != nullptr) {
        AnimatedSpriteComponent* animatedSpriteComponent = animated_sprite_component_copy((AnimatedSpriteComponent*) node->components[ComponentDataIndex_ANIMATED_SPRITE]);
        sceneNode->components[ComponentDataIndex_ANIMATED_SPRITE] = animatedSpriteComponent;
    }
    if (node->components[ComponentDataIndex_TEXT_LABEL] != nullptr) {
        TextLabelComponent* textLabelComponent = text_label_component_copy((TextLabelComponent*) node->components[ComponentDataIndex_TEXT_LABEL]);
        sceneNode->components[ComponentDataIndex_TEXT_LABEL] = textLabelComponent;
    }
    if (node->components[ComponentDataIndex_SCRIPT] != nullptr) {
        ScriptComponent* scriptComponent = script_component_copy((ScriptComponent*) node->components[ComponentDataIndex_SCRIPT]);
        sceneNode->components[ComponentDataIndex_SCRIPT] = scriptComponent;
    }
    if (node->components[ComponentDataIndex_COLLIDER_2D] != nullptr) {
        Collider2DComponent* collider2DComponent = collider2d_component_copy((Collider2DComponent*) node->components[ComponentDataIndex_COLLIDER_2D]);
        sceneNode->components[ComponentDataIndex_COLLIDER_2D] = collider2DComponent;
    }
    if (node->components[ComponentDataIndex_COLOR_SQUARE] != nullptr) {
        ColorSquareComponent* colorSquareComponent = color_square_component_copy((ColorSquareComponent*) node->components[ComponentDataIndex_COLOR_SQUARE]);
        sceneNode->components[ComponentDataIndex_COLOR_SQUARE] = colorSquareComponent;
    }

    // Load children
    for (size_t i = 0; i < node->childrenCount; i++) {
        SceneNode* childSceneNode = LoadSceneTreeNode(node->children[i], sceneNode);
        sceneNode->children.emplace_back(childSceneNode);
    }
    return sceneNode;
}
