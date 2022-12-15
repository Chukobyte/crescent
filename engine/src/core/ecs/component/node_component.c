#include "node_component.h"

#include <string.h>

#include "../seika/src/memory/se_mem.h"

NodeComponent* node_component_create() {
    NodeComponent* nodeComponent = SE_MEM_ALLOCATE(NodeComponent);
    nodeComponent->name[0] = '\0';
    nodeComponent->type = NodeBaseType_INVALID;
    nodeComponent->timeDilation = (NodeTimeDilation) {
        .value = 1.0f, .cachedFullValue = 1.0f, .cacheInvalid = true
    };
    nodeComponent->onSceneTreeEnter.observerCount = 0;
    nodeComponent->onSceneTreeExit.observerCount = 0;
    nodeComponent->observer.on_notify = NULL;
    return nodeComponent;
}

void node_component_delete(NodeComponent* nodeComponent) {
    SE_MEM_FREE(nodeComponent);
}

NodeComponent* node_component_copy(const NodeComponent* nodeComponent) {
    NodeComponent* copiedNode = SE_MEM_ALLOCATE(NodeComponent);
    memcpy(copiedNode, nodeComponent, sizeof(NodeComponent));
    return copiedNode;
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
    } else if (strcmp(baseName, RBE_NODE_COLOR_RECT_STRING) == 0) {
        return NodeBaseType_COLOR_RECT;
    } else if (strcmp(baseName, RBE_NODE_PARALLAX_STRING) == 0) {
        return NodeBaseType_PARALLAX;
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
    case NodeBaseType_COLOR_RECT:
        return NodeBaseInheritanceType_COLOR_RECT;
    case NodeBaseType_PARALLAX:
        return NodeBaseInheritanceType_PARALLAX;
    default:
        break;
    }
    return NodeBaseInheritanceType_INVALID;
}

const char* node_get_base_type_string(NodeBaseType type) {
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
    case NodeBaseType_COLOR_RECT:
        return RBE_NODE_COLOR_RECT_STRING;
    case NodeBaseType_PARALLAX:
        return RBE_NODE_PARALLAX_STRING;
    default:
        break;
    }
    return NULL;
}
