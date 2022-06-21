#pragma once

typedef enum NodeBaseType {
    NodeBaseType_INVALID = -1,
    NodeBaseType_NODE = 0,
    NodeBaseType_NODE2D = 1,
    NodeBaseType_SPRITE = 2,
    NodeBaseType_ANIMATED_SPRITE = 3,
    NodeBaseType_TEXT_LABEL = 4,
} NodeBaseType;

typedef enum NodeBaseInheritanceType {
    NodeBaseInheritanceType_INVALID = -1,
    NodeBaseInheritanceType_NODE = NodeBaseType_NODE,
    NodeBaseInheritanceType_NODE2D = NodeBaseType_NODE | NodeBaseType_NODE2D,
    NodeBaseInheritanceType_SPRITE = NodeBaseType_NODE | NodeBaseType_NODE2D | NodeBaseType_SPRITE,
    NodeBaseInheritanceType_ANIMATED_SPRITE = NodeBaseType_NODE | NodeBaseType_NODE2D | NodeBaseType_ANIMATED_SPRITE,
    NodeBaseInheritanceType_TEXT_LABEL = NodeBaseType_NODE | NodeBaseType_NODE2D | NodeBaseType_TEXT_LABEL,
} NodeBaseInheritanceType;

typedef struct NodeComponent {
    char name[32];
    NodeBaseType type;
} NodeComponent;

NodeComponent* node_component_create();
NodeBaseType node_get_base_type(const char* baseName);
NodeBaseInheritanceType node_get_type_inheritance(NodeBaseType type);
const char* node_get_component_type_string(NodeBaseType type);
