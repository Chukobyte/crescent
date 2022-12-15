#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include "../seika/src/utils/observer.h"
#include "../seika/src/data_structures/se_hash_map_string.h"
#include "../entity/entity.h"

#define RBE_NODE_NODE_STRING "Node"
#define RBE_NODE_NODE2D_STRING "Node2D"
#define RBE_NODE_SPRITE_STRING "Sprite"
#define RBE_NODE_ANIMATED_SPRITE_STRING "AnimatedSprite"
#define RBE_NODE_TEXT_LABEL_STRING "TextLabel"
#define RBE_NODE_COLLIDER2D_STRING "Collider2D"
#define RBE_NODE_COLOR_RECT_STRING "ColorRect"
#define RBE_NODE_PARALLAX_STRING "Parallax"

typedef enum NodeBaseType {
    NodeBaseType_INVALID = -1,
    NodeBaseType_NODE = 1 << 0,
    NodeBaseType_NODE2D = 1 << 1,
    NodeBaseType_SPRITE = 1 << 2,
    NodeBaseType_ANIMATED_SPRITE = 1 << 3,
    NodeBaseType_TEXT_LABEL = 1 << 4,
    NodeBaseType_COLLIDER2D = 1 << 5,
    NodeBaseType_COLOR_RECT = 1 << 6,
    NodeBaseType_PARALLAX = 1 << 7,
} NodeBaseType;

typedef enum NodeBaseInheritanceType {
    NodeBaseInheritanceType_INVALID = -1,
    NodeBaseInheritanceType_NODE = NodeBaseType_NODE,
    NodeBaseInheritanceType_NODE2D = NodeBaseType_NODE | NodeBaseType_NODE2D,
    NodeBaseInheritanceType_SPRITE = NodeBaseType_NODE | NodeBaseType_NODE2D | NodeBaseType_SPRITE,
    NodeBaseInheritanceType_ANIMATED_SPRITE = NodeBaseType_NODE | NodeBaseType_NODE2D | NodeBaseType_ANIMATED_SPRITE,
    NodeBaseInheritanceType_TEXT_LABEL = NodeBaseType_NODE | NodeBaseType_NODE2D | NodeBaseType_TEXT_LABEL,
    NodeBaseInheritanceType_COLLIDER2D = NodeBaseType_NODE | NodeBaseType_NODE2D | NodeBaseType_COLLIDER2D,
    NodeBaseInheritanceType_COLOR_RECT = NodeBaseType_NODE | NodeBaseType_NODE2D | NodeBaseType_COLOR_RECT,
    NodeBaseInheritanceType_PARALLAX = NodeBaseType_NODE | NodeBaseType_NODE2D | NodeBaseType_PARALLAX,
} NodeBaseInheritanceType;

typedef struct NodeTimeDilation {
    float value;
    float cachedFullValue;
    bool cacheInvalid;
} NodeTimeDilation;

// AKA Scene Component
typedef struct NodeComponent {
    char name[32];
    NodeBaseType type;
    NodeTimeDilation timeDilation;
    // Called after '_start' is called on an entity
    SEEvent onSceneTreeEnter; // { data = entity (unsigned int), type = 0 (not used) }
    // Called before '_end' is called on an entity
    SEEvent onSceneTreeExit; // { data = entity (unsigned int), type = 0 (not used) }
    // Observer used for all node events.  Other objects can use this (from scripting) to scope a callback to an entity's lifetime.
    SEObserver observer;
} NodeComponent;

NodeComponent* node_component_create();
void node_component_delete(NodeComponent* nodeComponent);
NodeComponent* node_component_copy(const NodeComponent* nodeComponent);
NodeBaseType node_get_base_type(const char* baseName);
NodeBaseInheritanceType node_get_type_inheritance(NodeBaseType type);
const char* node_get_base_type_string(NodeBaseType type);

#ifdef __cplusplus
}
#endif
