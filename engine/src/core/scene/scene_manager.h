#pragma once

#include "../ecs/entity/entity.h"
#include "../ecs/component/transform2d_component.h"

#define CGLM_DEFINE_PRINTS

#include <cglm/cglm.h>

// --- Scene Tree --- //
// Maintains parent child relationship between nodes
typedef struct SceneTreeNode {
    Entity entity;
    struct SceneTreeNode* parent;
    struct SceneTreeNode* children[MAX_ENTITIES / 2]; // TODO: Clean up temp
    size_t childCount;
} SceneTreeNode;

SceneTreeNode* rbe_scene_tree_create_tree_node(Entity entity, SceneTreeNode* parent);

// --- Scene Manager --- //
void rbe_scene_manager_initialize();
void rbe_scene_manager_finalize();
void rbe_scene_manager_queue_entity_for_creation(SceneTreeNode* treeNode);
void rbe_scene_manager_process_queued_creation_entities();
void rbe_scene_manager_queue_entity_for_deletion(Entity entity);
void rbe_scene_manager_process_queued_deletion_entities();
void rbe_scene_manager_queue_scene_change(const char* scenePath);
void rbe_scene_manager_process_queued_scene_change();

// Scene Tree related stuff, may separate into separate functionality later.
void rbe_scene_manager_set_active_scene_root(SceneTreeNode* root);
//struct Transform2DComponent rbe_scene_manager_get_combined_parent_transform(Entity entity);
//void rbe_scene_manager_update_global_tranform_model(Entity entity, mat4 model);
TransformModel2D* rbe_scene_manager_get_scene_node_global_transform(Entity entity, Transform2DComponent* transform2DComponent);
Entity rbe_scene_manager_get_entity_child_by_name(Entity parent, const char* childName);
SceneTreeNode* rbe_scene_manager_get_entity_tree_node(Entity entity);
