#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "scene_tree.h"
#include "../ecs/component/transform2d_component.h"
#include "../seika/src/math/se_math.h"

// --- Scene Tree --- //
SceneTreeNode* cre_scene_tree_create_tree_node(Entity entity, SceneTreeNode* parent);

// --- Scene Manager --- //
void cre_scene_manager_initialize();
void cre_scene_manager_finalize();
void cre_scene_manager_queue_entity_for_creation(SceneTreeNode* treeNode);
void cre_scene_manager_process_queued_creation_entities();
void cre_scene_manager_queue_entity_for_deletion(Entity entity);
void cre_queue_destroy_tree_node_entity_all(SceneTreeNode* treeNode);
void cre_scene_manager_process_queued_deletion_entities();
void cre_scene_manager_queue_scene_change(const char* scenePath);
void cre_scene_manager_process_queued_scene_change();

// Scene Tree related stuff, may separate into separate functionality later.
void cre_scene_manager_set_active_scene_root(SceneTreeNode* root);
SceneTreeNode* cre_scene_manager_get_active_scene_root();
TransformModel2D* cre_scene_manager_get_scene_node_global_transform(Entity entity, Transform2DComponent* transform2DComponent);
Entity cre_scene_manager_get_entity_child_by_name(Entity parent, const char* childName);
SceneTreeNode* cre_scene_manager_get_entity_tree_node(Entity entity);

#ifdef __cplusplus
}
#endif
