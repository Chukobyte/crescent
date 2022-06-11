#include "scene_manager.h"

#include "../scripting/script_context.h"
#include "../scripting/python/py_helper.h"
#include "../memory/rbe_mem.h"
#include "../ecs/component/component.h"
#include "../utils/logger.h"
#include "../utils/rbe_assert.h"

// --- Scene Tree --- //
// Maintains parent child relationship between nodes
typedef struct SceneTreeNode {
    Entity entity;
    struct SceneTreeNode* parent;
    struct SceneTreeNode** children;
    size_t childCount;
} SceneTreeNode;

typedef void (*ExecuteOnAllTreeNodesFunc) (SceneTreeNode*);

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

// --- Scene --- //
typedef struct Scene {
    const char* scenePath;
    SceneTree* sceneTree;
} Scene;

Scene* rbe_scene_create_scene(const char* scenePath) {
    Scene* scene = RBE_MEM_ALLOCATE(Scene);
    scene->scenePath = scenePath;
    scene->sceneTree = NULL;
    return scene;
}

// --- Scene Manager --- //
Entity entitiesQueuedForCreation[MAX_ENTITIES];
size_t entitiesQueuedForCreationSize = 0;
Entity entitiesQueuedForDeletion[MAX_ENTITIES];
size_t entitiesQueuedForDeletionSize = 0;

RBEScriptContext* sceneManagerScriptContext = NULL;
Scene* activeScene = NULL;
Scene* queuedSceneToChangeTo = NULL;

void rbe_scene_manager_initialize() {
    sceneManagerScriptContext = rbe_script_context_get_current();
}

void rbe_scene_manager_finalize() {}

void rbe_scene_manager_queue_entity_for_creation(Entity entity) {
    entitiesQueuedForCreation[entitiesQueuedForCreationSize++] = entity;
}

void rbe_scene_manager_process_queued_creation_entities() {
    for (size_t i = 0; i < entitiesQueuedForCreationSize; i++) {
        sceneManagerScriptContext->on_start(entitiesQueuedForCreation[i]);
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
    sceneManagerScriptContext->on_end(entity);
}

void rbe_scene_manager_process_queued_deletion_entities() {
    for (size_t i = 0; i < entitiesQueuedForDeletionSize; i++) {
        sceneManagerScriptContext->on_delete_instance(entitiesQueuedForDeletion[i]);
        component_manager_remove_all_components(entitiesQueuedForDeletion[i]);
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
        activeScene = queuedSceneToChangeTo;
        queuedSceneToChangeTo = NULL;
        RBE_ASSERT(activeScene->scenePath != NULL);
        // Queues entities for creation
        pyh_run_python_file(activeScene->scenePath);
    }
}
