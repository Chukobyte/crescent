#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../../ecs/component/component.h"
#include "../../ecs/component/node_component.h"

#include "../../animation/animation.h"
#include "../../game_properties.h"
#include "../../scene/scene_tree.h"

struct _object; // PyObject

// File Scene Node
typedef struct FileSceneNodeAnimatedSpritePaths {
    char* data[RBE_MAX_ANIMATION_FRAMES][RBE_MAX_ANIMATION_FRAMES];
} FileSceneNodeAnimatedSpritePaths;

typedef struct FileSceneNode {
    char* name;
    NodeBaseType type;
    void* components[MAX_COMPONENTS];
    struct FileSceneNode* parent;
    struct FileSceneNode* children[MAX_ENTITIES / 4];
    size_t childrenCount;
    char* spriteTexturePath;
    char* fontUID;
} FileSceneNode;

FileSceneNode* file_scene_node_create_cached_file_scene_nodes_from_list(struct _object* stageNodeList);
void file_scene_node_delete_cached_file_scene_node();
FileSceneNode* file_scene_node_get_cached_file_scene_node();

// Py File Loader
RBEGameProperties* rbe_py_load_game_properties(const char* filePath);

#ifdef __cplusplus
}
#endif
