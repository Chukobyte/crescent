#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../seika/math/se_math.h"

#include "scene_tree.h"
#include "../ecs/component/transform2d_component.h"
#include "../json/json_file_loader.h"

// Scene Tree
// Creates a new tree node.  If NULL is passed in for parent, it's expected that it's the root of a scene tree.
SceneTreeNode* cre_scene_tree_create_tree_node(CreEntity entity, SceneTreeNode* parent);

// Scene Manager
typedef void (*OnNodeEnteredSceneFunc) (CreEntity);

typedef struct SceneNodeCallbackSubscriber {
    OnNodeEnteredSceneFunc onNodeEnteredSceneFunc;
} SceneNodeCallbackSubscriber;

// First index is the child
typedef struct EntityArray {
    int entityCount;
    CreEntity entities[10];
} EntityArray;

void cre_scene_manager_initialize();
void cre_scene_manager_finalize();
// Will add entity to the scene upon the beginning of the next frame
void cre_scene_manager_queue_node_for_creation(SceneTreeNode* treeNode);
// Will stage a node to be added as a child at a later time (e.g. creating a new node instance)
void cre_scene_manager_stage_child_node_to_be_added_later(SceneTreeNode* treeNode);
SceneTreeNode* cre_scene_manager_stage_scene_nodes_from_json(JsonSceneNode* jsonSceneNode);
void cre_scene_manager_process_queued_creation_entities();
void cre_scene_manager_queue_entity_for_deletion(CreEntity entity);
void cre_queue_destroy_tree_node_entity_all(SceneTreeNode* treeNode);
void cre_scene_manager_process_queued_deletion_entities();
void cre_scene_manager_queue_scene_change(const char* scenePath);
void cre_scene_manager_process_queued_scene_change();

// Scene Tree related stuff, may separate into separate functionality later.
void cre_scene_manager_set_active_scene_root(SceneTreeNode* root);
SceneTreeNode* cre_scene_manager_get_active_scene_root();
SETransformModel2D* cre_scene_manager_get_scene_node_global_transform(CreEntity entity, Transform2DComponent* transform2DComponent);
float cre_scene_manager_get_node_full_time_dilation(CreEntity entity);
CreEntity cre_scene_manager_get_entity_child_by_name(CreEntity parent, const char* childName);
SceneTreeNode* cre_scene_manager_get_entity_tree_node(CreEntity entity);
bool cre_scene_manager_has_entity_tree_node(CreEntity entity);
void cre_scene_manager_add_node_as_child(CreEntity childEntity, CreEntity parentEntity);
EntityArray cre_scene_manager_get_self_and_parent_nodes(CreEntity entity);
void cre_scene_manager_invalidate_time_dilation_nodes_with_children(CreEntity entity);

// Helper function to call notify on entity and children node 'on transform changed' events.  Uses recursion.
void cre_scene_manager_notify_all_on_transform_events(CreEntity entity, Transform2DComponent* transformComp);

typedef void (*ExecuteOnAllTreeNodesFunc) (SceneTreeNode*);
void cre_scene_manager_execute_on_root_and_child_nodes(ExecuteOnAllTreeNodesFunc func);

#ifdef __cplusplus
}
#endif
