#include "scene_manager.h"

#include "../engine/src/core/ecs/component/component.h"
#include "../engine/src/core/utils/logger.h"
#include "../engine/src/core/utils/rbe_assert.h"

bool SceneManager::LoadSceneFromFile(const char *sceneFilePath) {
    if (SceneTreeNode* rootTreeNode = rbe_py_load_scene(sceneFilePath)) {
//    if (SceneTreeNode* rootTreeNode = rbe_scene_manager_get_active_scene_root()) {
        SceneNodeFile* nodeFile = GenerateSceneNodeFile(rootTreeNode, sceneFilePath);
        RBE_ASSERT_FMT(nodeFile != nullptr, "Node file at path '%s' is invalid", sceneFilePath);
        loadedSceneFiles.emplace_back(nodeFile);
        return true;
    }
    return false;
}

SceneNodeFile *SceneManager::GenerateSceneNodeFile(SceneTreeNode *rootTreeNode, const char* sceneFilePath) {
    RBE_ASSERT_FMT(rootTreeNode->parent == nullptr, "Scene tree root has a parent!?  At path '%s'", sceneFilePath);
    SceneNodeFile* nodeFile = new SceneNodeFile{ sceneFilePath };
    nodeFile->rootNode = LoadSceneTreeNode(rootTreeNode);
    return nodeFile;
}

SceneNode* SceneManager::LoadSceneTreeNode(SceneTreeNode* node, SceneNode* parent) {
    SceneNode* sceneNode = new SceneNode();
    sceneNode->parent = parent;

    if (NodeComponent* nodeComponent = (NodeComponent*) component_manager_get_component_unsafe(node->entity, ComponentDataIndex_NODE)) {
        sceneNode->name = nodeComponent->name;
        sceneNode->type = nodeComponent->type;
    } else {
        rbe_logger_error("Doesn't have node component!?");
    }

    // Load children
    for (size_t i = 0; i < node->childCount; i++) {
        SceneNode* childSceneNode = LoadSceneTreeNode(node->children[i], sceneNode);
        sceneNode->children.emplace_back(childSceneNode);
    }
    return sceneNode;
}
