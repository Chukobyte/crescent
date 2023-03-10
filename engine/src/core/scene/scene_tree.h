#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#include "../ecs/entity/entity.h"

#define SCENE_TREE_NODE_MAX_CHILDREN (CRE_MAX_ENTITIES - 1)

// Maintains parent child relationship between nodes
typedef struct SceneTreeNode {
    CreEntity entity;
    struct SceneTreeNode* parent;
    struct SceneTreeNode* children[SCENE_TREE_NODE_MAX_CHILDREN];
    size_t childCount;
} SceneTreeNode;

#ifdef __cplusplus
}
#endif
