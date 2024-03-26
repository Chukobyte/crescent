#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include <seika/event.h>
#include <seika/data_structures/hash_map_string.h>

#define CRE_NODE_NODE_STRING "Node"
#define CRE_NODE_NODE2D_STRING "Node2D"
#define CRE_NODE_SPRITE_STRING "Sprite"
#define CRE_NODE_ANIMATED_SPRITE_STRING "AnimatedSprite"
#define CRE_NODE_TEXT_LABEL_STRING "TextLabel"
#define CRE_NODE_COLLIDER2D_STRING "Collider2D"
#define CRE_NODE_COLOR_RECT_STRING "ColorRect"
#define CRE_NODE_PARALLAX_STRING "Parallax"
#define CRE_NODE_PARTICLES2D_STRING "Particles2D"
#define CRE_NODE_TILEMAP_STRING "Tilemap"

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
    NodeBaseType_PARTICLES2D = 1 << 8,
    NodeBaseType_TILEMAP = 1 << 9,
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
    NodeBaseInheritanceType_PARTICLES2D = NodeBaseType_NODE | NodeBaseType_NODE2D | NodeBaseType_PARTICLES2D,
    NodeBaseInheritanceType_TILEMAP = NodeBaseType_NODE | NodeBaseType_NODE2D | NodeBaseType_TILEMAP,
} NodeBaseInheritanceType;

typedef struct NodeTimeDilation {
    f32 value;
    f32 cachedFullValue;
    bool cacheInvalid;
} NodeTimeDilation;

// AKA Scene Component
typedef struct NodeComponent {
    char name[32];
    NodeBaseType type;
    bool queuedForDeletion;
    NodeTimeDilation timeDilation;
    // Called after '_start' is called on an entity
    SkaEvent onSceneTreeEnter; // { data = entity (unsigned int), type = 0 (not used) }
    // Called before '_end' is called on an entity
    SkaEvent onSceneTreeExit; // { data = entity (unsigned int), type = 0 (not used) }
} NodeComponent;

NodeComponent* node_component_create();
NodeComponent* node_component_create_ex(const char* name, NodeBaseType baseType);
void node_component_delete(NodeComponent* nodeComponent);
NodeComponent* node_component_copy(const NodeComponent* nodeComponent);
NodeBaseType node_get_base_type(const char* baseName);
NodeBaseInheritanceType node_get_type_inheritance(NodeBaseType type);
const char* node_get_base_type_string(NodeBaseType type);

#ifdef __cplusplus
}
#endif
