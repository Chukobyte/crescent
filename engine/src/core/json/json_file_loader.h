#pragma once

#include "../ecs/entity/entity.h"
#include "../ecs/component/component.h"
#include "../ecs/component/node_component.h"

// TODO: Decide if we want a callback function to load scene files...
//typedef void (*OnSceneNodeLoad) (SceneNode*);

typedef struct JsonSceneNode {
    char* name;
    NodeBaseType type;
    void* components[MAX_COMPONENTS];
    struct JsonSceneNode* parent;
    struct JsonSceneNode* children[MAX_ENTITIES / 4];
    size_t childrenCount;
    char* spriteTexturePath;
    char* fontUID;
} JsonSceneNode;

struct RBEGameProperties* cre_json_load_config_file(const char* filePath);
JsonSceneNode* cre_json_load_scene_file(const char* filePath);
void cre_json_delete_json_scene_node(JsonSceneNode* node);
