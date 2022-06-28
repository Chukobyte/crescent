#include "scene_manager.h"
#include <string.h>

#include "../data_structures/rbe_hash_map.h"
#include "../ecs/component/component.h"
#include "../ecs/component/node_component.h"
#include "../ecs/component/transform2d_component.h"
#include "../ecs/system/ec_system.h"
#include "../memory/rbe_mem.h"
#include "../scripting/python/py_helper.h"
#include "../utils/logger.h"
#include "../utils/rbe_assert.h"

// --- Scene Tree --- //
typedef void (*ExecuteOnAllTreeNodesFunc)(SceneTreeNode*);

// Executes function on passed in tree node and all child tree nodes
void rbe_scene_execute_on_all_tree_nodes(SceneTreeNode* treeNode, ExecuteOnAllTreeNodesFunc func) {
    for (size_t i = 0; i < treeNode->childCount; i++) {
        rbe_scene_execute_on_all_tree_nodes(treeNode->children[i], func);
    }
    func(treeNode);
}

typedef struct SceneTree {
    SceneTreeNode* root;
} SceneTree;

SceneTreeNode* rbe_scene_tree_create_tree_node(Entity entity, SceneTreeNode* parent) {
    SceneTreeNode* treeNode = RBE_MEM_ALLOCATE(SceneTreeNode);
    treeNode->entity        = entity;
    treeNode->parent        = parent;
    treeNode->childCount    = 0;
    return treeNode;
}

// --- Scene --- //
typedef struct Scene {
    const char* scenePath;
    SceneTree* sceneTree;
} Scene;

Scene* rbe_scene_create_scene(const char* scenePath) {
    Scene* scene           = RBE_MEM_ALLOCATE(Scene);
    scene->scenePath       = scenePath;
    scene->sceneTree       = RBE_MEM_ALLOCATE(SceneTree);
    scene->sceneTree->root = NULL;
    return scene;
}

// --- Scene Manager --- //
Entity entitiesQueuedForCreation[MAX_ENTITIES];
size_t entitiesQueuedForCreationSize = 0;
Entity entitiesQueuedForDeletion[MAX_ENTITIES];
size_t entitiesQueuedForDeletionSize = 0;

Scene* activeScene           = NULL;
Scene* queuedSceneToChangeTo = NULL;

static RBEHashMap* entityToTreeNodeMap = NULL;

void rbe_scene_manager_initialize() {
    RBE_ASSERT(entityToTreeNodeMap == NULL);
    entityToTreeNodeMap = rbe_hash_map_create(sizeof(Entity), sizeof(SceneTreeNode), 16); // TODO: Update capacity
}

void rbe_scene_manager_finalize() {
    RBE_ASSERT(entityToTreeNodeMap != NULL);
    rbe_hash_map_destroy(entityToTreeNodeMap);
}

void rbe_scene_manager_queue_entity_for_creation(SceneTreeNode* treeNode) {
    entitiesQueuedForCreation[entitiesQueuedForCreationSize++] = treeNode->entity;
    RBE_ASSERT_FMT(!rbe_hash_map_has(entityToTreeNodeMap, &treeNode->entity),
                   "Entity '%d' already in entity to tree map!", treeNode->entity);
    rbe_hash_map_add(entityToTreeNodeMap, &treeNode->entity, treeNode);
}

void rbe_scene_manager_process_queued_creation_entities() {
    for (size_t i = 0; i < entitiesQueuedForCreationSize; i++) {
        rbe_ec_system_entity_start(entitiesQueuedForCreation[i]);
    }
    entitiesQueuedForCreationSize = 0;
}

void rbe_scene_manager_queue_entity_for_deletion(Entity entity) {
    // Check if entity is already queued exit out early if so
    for (size_t i = 0; i < entitiesQueuedForDeletionSize; i++) {
        if (entitiesQueuedForDeletion[i] == entity) {
            rbe_logger_warn("Entity '%d' already queued for deletion!", entity);
            return;
        }
    }
    // Insert queued entity
    entitiesQueuedForDeletion[entitiesQueuedForDeletionSize++] = entity;
    // Clean up
    rbe_ec_system_entity_end(entity);
}

void rbe_scene_manager_process_queued_deletion_entities() {
    for (size_t i = 0; i < entitiesQueuedForDeletionSize; i++) {
        // Remove entity from entity to tree node map
        Entity entityToDelete = entitiesQueuedForDeletion[i];
        RBE_ASSERT_FMT(rbe_hash_map_has(entityToTreeNodeMap, &entityToDelete), "Entity '%d' not in tree node map!?",
                       entityToDelete);
        SceneTreeNode* treeNode = rbe_hash_map_get(entityToTreeNodeMap, &entityToDelete);
        //        RBE_MEM_FREE(treeNode); // TODO: Crashes rbe_hash_map_erase, investigate...
        rbe_hash_map_erase(entityToTreeNodeMap, &entityToDelete);
        // Remove entity from systems
        rbe_ec_system_remove_entity_from_all_systems(entityToDelete);
        // Remove all components
        component_manager_remove_all_components(entityToDelete);
    }
    entitiesQueuedForDeletionSize = 0;
}

void rbe_scene_manager_queue_scene_change(const char* scenePath) {
    if (queuedSceneToChangeTo == NULL) {
        queuedSceneToChangeTo = rbe_scene_create_scene(scenePath);
    } else {
        rbe_logger_warn("Scene already queued, not loading '%s'", scenePath);
    }
}

void rbe_queue_destroy_tree_node_entity(SceneTreeNode* treeNode) {
    rbe_scene_manager_queue_entity_for_deletion(treeNode->entity);
    RBE_MEM_FREE(treeNode);
}

void rbe_scene_manager_process_queued_scene_change() {
    if (queuedSceneToChangeTo != NULL) {
        // Destroy old scene
        if (activeScene != NULL) {
            rbe_scene_execute_on_all_tree_nodes(activeScene->sceneTree->root, rbe_queue_destroy_tree_node_entity);
            RBE_MEM_FREE(activeScene);
        }
        // Setup new scene
        activeScene           = queuedSceneToChangeTo;
        queuedSceneToChangeTo = NULL;
        RBE_ASSERT(activeScene->scenePath != NULL);
        // Queues entities for creation
        pyh_run_python_file(activeScene->scenePath);
    }
}

void rbe_scene_manager_set_active_scene_root(SceneTreeNode* root) {
    RBE_ASSERT(activeScene != NULL);
    RBE_ASSERT_FMT(activeScene->sceneTree->root == NULL, "Trying to overwrite an already existing scene root!");
    activeScene->sceneTree->root = root;
}

Transform2DComponent rbe_scene_manager_get_combined_parent_transform(Entity entity) {
    SceneTreeNode* sceneTreeNode              = rbe_scene_manager_get_entity_tree_node(entity);
    Transform2DComponent transform2DComponent = {
        .position                 = {.x = 0.0f, .y = 0.0f},
        .scale                    = {.x = 1.0f, .y = 1.0f},
        .rotation                 = 0.0f,
        .zIndex                   = 0,
        .ignoreCamera             = false,
        .isZIndexRelativeToParent = true,
    };
    SceneTreeNode* parentTreeNode = sceneTreeNode->parent;
    while (parentTreeNode != NULL) {
        Transform2DComponent* parentTransform2DComponent =
            component_manager_get_component_unsafe(parentTreeNode->entity, ComponentDataIndex_TRANSFORM_2D);
        // If transform 2D is missing, will continue walk up the tree.  This is to allow some flexibility in case
        // non node2d parent child relationships are introduced.
        if (parentTransform2DComponent == NULL) {
            parentTreeNode = parentTreeNode->parent;
            continue;
        }
        transform2DComponent.position.x += parentTransform2DComponent->position.x;
        transform2DComponent.position.y += parentTransform2DComponent->position.y;
        transform2DComponent.scale.x *= parentTransform2DComponent->scale.x;
        transform2DComponent.scale.y *= parentTransform2DComponent->scale.y;
        transform2DComponent.zIndex += parentTransform2DComponent->zIndex;
        parentTreeNode = parentTreeNode->parent;
    }
    return transform2DComponent;
}

SceneTreeNode* rbe_scene_manager_get_entity_tree_node(Entity entity) {
    RBE_ASSERT(rbe_hash_map_has(entityToTreeNodeMap, &entity));
    SceneTreeNode* treeNode = (SceneTreeNode*) rbe_hash_map_get(entityToTreeNodeMap, &entity);
    RBE_ASSERT(treeNode != NULL);
    return treeNode;
}

Entity rbe_scene_manager_get_entity_child_by_name(Entity parent, const char* childName) {
    SceneTreeNode* parentNode = rbe_scene_manager_get_entity_tree_node(parent);
    RBE_ASSERT(parentNode != NULL);
    for (size_t childIndex = 0; childIndex < parentNode->childCount; childIndex++) {
        const Entity childEntity = parentNode->children[childIndex]->entity;
        if (component_manager_has_component(childEntity, ComponentDataIndex_NODE)) {
            NodeComponent* childNodeComponent = component_manager_get_component(childEntity, ComponentDataIndex_NODE);
            if (strcmp(childNodeComponent->name, childName) == 0) {
                return childEntity;
            }
        }
    }
    return NULL_ENTITY;
}
