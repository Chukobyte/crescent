#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <Python.h>

#include "../../ecs/component/component.h"
#include "../../ecs/component/node_component.h"
#include "../../game_properties.h"
#include "../../scene/scene_tree.h"

// File Scene Node
typedef struct FileSceneNode {
    char* name;
    NodeBaseType type;
    void* components[MAX_COMPONENTS];
    struct FileSceneNode* parent;
    struct FileSceneNode* children[MAX_ENTITIES / 4];
    size_t childrenCount;
} FileSceneNode;

FileSceneNode* file_scene_node_create_cached_file_scene_nodes_from_list(PyObject* stageNodeList);
void file_scene_node_delete_cached_file_scene_node();
FileSceneNode* file_scene_node_get_cached_file_scene_node();
void* file_scene_node_get_component(FileSceneNode* node, ComponentDataIndex index);

// Py File Loader
RBEGameProperties* rbe_py_load_game_properties(const char* filePath);
SceneTreeNode* rbe_py_load_scene(const char* filePath);

#ifdef __cplusplus
}
#endif
