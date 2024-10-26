#include "node_component.h"

#include <string.h>

#include <seika/memory.h>
#include <seika/assert.h>
#include <seika/string.h>

NodeComponent* node_component_create() {
    NodeComponent* nodeComponent = SKA_ALLOC_ZEROED(NodeComponent);
    nodeComponent->name[0] = '\0';
    nodeComponent->timeDilation = (NodeTimeDilation){ .value = 1.0f, .cachedFullValue = 1.0f, .cacheInvalid = true };
    return nodeComponent;
}

NodeComponent* node_component_create_ex(const char* name, NodeBaseType baseType) {
    NodeComponent* nodeComponent = node_component_create();
    ska_strcpy(nodeComponent->name, name);
    nodeComponent->type = baseType;
    return nodeComponent;
}

void node_component_delete(NodeComponent* nodeComponent) {
    SKA_FREE(nodeComponent);
}

NodeComponent* node_component_copy(const NodeComponent* nodeComponent) {
    NodeComponent* copiedNode = SKA_ALLOC(NodeComponent);
    memcpy(copiedNode, nodeComponent, sizeof(NodeComponent));
    return copiedNode;
}

NodeBaseType node_get_base_type(const char* baseName) {
    if (strcmp(baseName, CRE_NODE_NODE_STRING) == 0) {
        return NodeBaseType_NODE;
    } else if (strcmp(baseName, CRE_NODE_NODE2D_STRING) == 0) {
        return NodeBaseType_NODE2D;
    } else if (strcmp(baseName, CRE_NODE_SPRITE_STRING) == 0) {
        return NodeBaseType_SPRITE;
    } else if (strcmp(baseName, CRE_NODE_ANIMATED_SPRITE_STRING) == 0) {
        return NodeBaseType_ANIMATED_SPRITE;
    } else if (strcmp(baseName, CRE_NODE_TEXT_LABEL_STRING) == 0) {
        return NodeBaseType_TEXT_LABEL;
    } else if (strcmp(baseName, CRE_NODE_COLLIDER2D_STRING) == 0) {
        return NodeBaseType_COLLIDER2D;
    } else if (strcmp(baseName, CRE_NODE_COLOR_RECT_STRING) == 0) {
        return NodeBaseType_COLOR_RECT;
    } else if (strcmp(baseName, CRE_NODE_PARALLAX_STRING) == 0) {
        return NodeBaseType_PARALLAX;
    } else if (strcmp(baseName, CRE_NODE_PARTICLES2D_STRING) == 0) {
        return NodeBaseType_PARTICLES2D;
    } else if (strcmp(baseName, CRE_NODE_TILEMAP_STRING) == 0) {
        return NodeBaseType_TILEMAP;
    }
    return NodeBaseType_INVALID;
}

NodeBaseInheritanceType node_get_type_inheritance(NodeBaseType type) {
    switch (type) {
    case NodeBaseType_NODE: return NodeBaseInheritanceType_NODE;
    case NodeBaseType_NODE2D: return NodeBaseInheritanceType_NODE2D;
    case NodeBaseType_SPRITE: return NodeBaseInheritanceType_SPRITE;
    case NodeBaseType_ANIMATED_SPRITE: return NodeBaseInheritanceType_ANIMATED_SPRITE;
    case NodeBaseType_TEXT_LABEL: return NodeBaseInheritanceType_TEXT_LABEL;
    case NodeBaseType_COLLIDER2D: return NodeBaseInheritanceType_COLLIDER2D;
    case NodeBaseType_COLOR_RECT: return NodeBaseInheritanceType_COLOR_RECT;
    case NodeBaseType_PARALLAX: return NodeBaseInheritanceType_PARALLAX;
    case NodeBaseType_PARTICLES2D: return NodeBaseInheritanceType_PARTICLES2D;
    case NodeBaseType_TILEMAP: return NodeBaseInheritanceType_TILEMAP;
    default: return NodeBaseInheritanceType_INVALID;
    }
}

const char* node_get_base_type_string(NodeBaseType type) {
    switch (type) {
    case NodeBaseType_NODE: return CRE_NODE_NODE_STRING;
    case NodeBaseType_NODE2D: return CRE_NODE_NODE2D_STRING;
    case NodeBaseType_SPRITE: return CRE_NODE_SPRITE_STRING;
    case NodeBaseType_ANIMATED_SPRITE: return CRE_NODE_ANIMATED_SPRITE_STRING;
    case NodeBaseType_TEXT_LABEL: return CRE_NODE_TEXT_LABEL_STRING;
    case NodeBaseType_COLLIDER2D: return CRE_NODE_COLLIDER2D_STRING;
    case NodeBaseType_COLOR_RECT: return CRE_NODE_COLOR_RECT_STRING;
    case NodeBaseType_PARALLAX: return CRE_NODE_PARALLAX_STRING;
    case NodeBaseType_PARTICLES2D: return CRE_NODE_PARTICLES2D_STRING;
    case NodeBaseType_TILEMAP: return CRE_NODE_TILEMAP_STRING;
    default: SKA_ASSERT_FMT(false, "Invalid node base type '%d'", type);
    }
    return NULL;
}
