#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#include "../ecs/entity/entity.h"

// Maintains parent child relationship between nodes
typedef struct SceneTreeNode {
    Entity entity;
    struct SceneTreeNode* parent;
    struct SceneTreeNode* children[MAX_ENTITIES / 2]; // TODO: Clean up temp
    size_t childCount;
} SceneTreeNode;

#ifdef __cplusplus
}
#endif
