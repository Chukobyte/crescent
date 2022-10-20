#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "scene_tree.h"
#include "../ecs/component/transform2d_component.h"
#include "../seika/src/math/rbe_math.h"

// --- Scene Tree --- //
SceneTreeNode* rbe_scene_tree_create_tree_node(Entity entity, SceneTreeNode* parent);

// --- Scene Manager --- //
void rbe_scene_manager_initialize();
void rbe_scene_manager_finalize();
void rbe_scene_manager_queue_entity_for_creation(SceneTreeNode* treeNode);
void rbe_scene_manager_process_queued_creation_entities();
void rbe_scene_manager_queue_entity_for_deletion(Entity entity);
void rbe_queue_destroy_tree_node_entity_all(SceneTreeNode* treeNode);
void rbe_scene_manager_process_queued_deletion_entities();
void rbe_scene_manager_queue_scene_change(const char* scenePath);
void rbe_scene_manager_process_queued_scene_change();

// Scene Tree related stuff, may separate into separate functionality later.
void rbe_scene_manager_set_active_scene_root(SceneTreeNode* root);
SceneTreeNode* rbe_scene_manager_get_active_scene_root();
TransformModel2D* rbe_scene_manager_get_scene_node_global_transform(Entity entity, Transform2DComponent* transform2DComponent);
Entity rbe_scene_manager_get_entity_child_by_name(Entity parent, const char* childName);
SceneTreeNode* rbe_scene_manager_get_entity_tree_node(Entity entity);

#ifdef __cplusplus
}
#endif
