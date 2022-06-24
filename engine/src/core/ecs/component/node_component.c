#include "node_component.h"

#include <string.h>

#include "../../memory/rbe_mem.h"

NodeComponent* node_component_create() {
    NodeComponent* nodeComponent = RBE_MEM_ALLOCATE(NodeComponent);
    nodeComponent->name[0] = '\0';
    nodeComponent->type = NodeBaseType_INVALID;
    return nodeComponent;
}

NodeBaseType node_get_base_type(const char* baseName) {
    if (strcmp(baseName, RBE_NODE_NODE_STRING) == 0) {
        return NodeBaseType_NODE;
    } else if (strcmp(baseName, RBE_NODE_NODE2D_STRING) == 0) {
        return NodeBaseType_NODE2D;
    } else if (strcmp(baseName, RBE_NODE_SPRITE_STRING) == 0) {
        return NodeBaseType_SPRITE;
    } else if (strcmp(baseName, RBE_NODE_ANIMATED_SPRITE_STRING) == 0) {
        return NodeBaseType_ANIMATED_SPRITE;
    } else if (strcmp(baseName, RBE_NODE_TEXT_LABEL_STRING) == 0) {
        return NodeBaseType_TEXT_LABEL;
    } else if (strcmp(baseName, RBE_NODE_COLLIDER2D_STRING) == 0) {
        return NodeBaseType_COLLIDER2D;
    }
    return NodeBaseType_INVALID;
}

NodeBaseInheritanceType node_get_type_inheritance(NodeBaseType type) {
    switch (type) {
    case NodeBaseType_NODE:
        return NodeBaseInheritanceType_NODE;
    case NodeBaseType_NODE2D:
        return NodeBaseInheritanceType_NODE2D;
    case NodeBaseType_SPRITE:
        return NodeBaseInheritanceType_SPRITE;
    case NodeBaseType_ANIMATED_SPRITE:
        return NodeBaseInheritanceType_ANIMATED_SPRITE;
    case NodeBaseType_TEXT_LABEL:
        return NodeBaseInheritanceType_TEXT_LABEL;
    case NodeBaseType_COLLIDER2D:
        return NodeBaseInheritanceType_COLLIDER2D;
    default:
        break;
    }
    return NodeBaseInheritanceType_INVALID;
}

const char* node_get_component_type_string(NodeBaseType type) {
    switch (type) {
    case NodeBaseType_NODE:
        return RBE_NODE_NODE_STRING;
    case NodeBaseType_NODE2D:
        return RBE_NODE_NODE2D_STRING;
    case NodeBaseType_SPRITE:
        return RBE_NODE_SPRITE_STRING;
    case NodeBaseType_ANIMATED_SPRITE:
        return RBE_NODE_ANIMATED_SPRITE_STRING;
    case NodeBaseType_TEXT_LABEL:
        return RBE_NODE_TEXT_LABEL_STRING;
    case NodeBaseType_COLLIDER2D:
        return RBE_NODE_COLLIDER2D_STRING;
    default:
        break;
    }
    return NULL;
}
