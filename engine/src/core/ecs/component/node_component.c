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
    if (strcmp(baseName, "Node") == 0) {
        return NodeBaseType_NODE;
    } else if (strcmp(baseName, "Node2D") == 0) {
        return NodeBaseType_NODE2D;
    } else if (strcmp(baseName, "Sprite") == 0) {
        return NodeBaseType_SPRITE;
    } else if (strcmp(baseName, "AnimatedSprite") == 0) {
        return NodeBaseType_ANIMATED_SPRITE;
    } else if (strcmp(baseName, "TextLabel") == 0) {
        return NodeBaseType_TEXT_LABEL;
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
    default:
        break;
    }
    return NodeBaseInheritanceType_INVALID;
}
