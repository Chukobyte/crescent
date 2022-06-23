#pragma once

#define RBE_NODE_NODE_STRING "Node"
#define RBE_NODE_NODE2D_STRING "Node2D"
#define RBE_NODE_SPRITE_STRING "Sprite"
#define RBE_NODE_ANIMATED_SPRITE_STRING "AnimatedSprite"
#define RBE_NODE_TEXT_LABEL_STRING "TextLabel"
#define RBE_NODE_COLLIDER2D_STRING "Collider2D"

typedef enum NodeBaseType {
    NodeBaseType_INVALID = -1,
    NodeBaseType_NODE = 0,
    NodeBaseType_NODE2D = 1,
    NodeBaseType_SPRITE = 2,
    NodeBaseType_ANIMATED_SPRITE = 3,
    NodeBaseType_TEXT_LABEL = 4,
    NodeBaseType_COLLIDER2D = 5,
} NodeBaseType;

typedef enum NodeBaseInheritanceType {
    NodeBaseInheritanceType_INVALID = -1,
    NodeBaseInheritanceType_NODE = NodeBaseType_NODE,
    NodeBaseInheritanceType_NODE2D = NodeBaseType_NODE | NodeBaseType_NODE2D,
    NodeBaseInheritanceType_SPRITE = NodeBaseType_NODE | NodeBaseType_NODE2D | NodeBaseType_SPRITE,
    NodeBaseInheritanceType_ANIMATED_SPRITE = NodeBaseType_NODE | NodeBaseType_NODE2D | NodeBaseType_ANIMATED_SPRITE,
    NodeBaseInheritanceType_TEXT_LABEL = NodeBaseType_NODE | NodeBaseType_NODE2D | NodeBaseType_TEXT_LABEL,
    NodeBaseInheritanceType_COLLIDER2D = NodeBaseType_NODE | NodeBaseType_NODE2D | NodeBaseType_COLLIDER2D,
} NodeBaseInheritanceType;

typedef struct NodeComponent {
    char name[32];
    NodeBaseType type;
} NodeComponent;

NodeComponent* node_component_create();
NodeBaseType node_get_base_type(const char* baseName);
NodeBaseInheritanceType node_get_type_inheritance(NodeBaseType type);
const char* node_get_component_type_string(NodeBaseType type);
