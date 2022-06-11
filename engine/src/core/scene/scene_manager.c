#include "scene_manager.h"

#include "../scripting/script_context.h"
#include "../utils/logger.h"
#include "../memory/rbe_mem.h"
#include "../utils/rbe_assert.h"
#include "../scripting/python/py_helper.h"

// --- Scene Tree --- //
typedef struct SceneTreeNode {
    Entity entity;
    Entity children[MAX_ENTITIES - 1];
    size_t children_size;
} SceneTreeNode;

typedef struct SceneTree {
    SceneTreeNode root;
} SceneTree;

// --- Scene --- //
typedef struct Scene {
    const char* scenePath;
    SceneTree sceneTree;
} Scene;

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
    entitiesQueuedForDeletion[entitiesQueuedForDeletionSize++] = entity;
}

void rbe_scene_manager_process_queued_deletion_entities() {
    for (size_t i = 0; i < entitiesQueuedForDeletionSize; i++) {
        sceneManagerScriptContext->on_end(entitiesQueuedForDeletion[i]);
    }
    entitiesQueuedForDeletionSize = 0;
}

void rbe_scene_manager_queue_scene_change(const char* scenePath) {
    if (queuedSceneToChangeTo == NULL) {
        queuedSceneToChangeTo = RBE_MEM_ALLOCATE(Scene);
        queuedSceneToChangeTo->scenePath = scenePath;
        queuedSceneToChangeTo->sceneTree.root.entity = NULL_ENTITY;
    } else {
        rbe_logger_warn("Scene already queued, not loading '%s'", scenePath);
    }
}

void rbe_scene_manager_process_queued_scene_change() {
    if (queuedSceneToChangeTo != NULL) {
        // Destroy old scene
        RBE_MEM_FREE(activeScene);
        // Setup new scene
        activeScene = queuedSceneToChangeTo;
        queuedSceneToChangeTo = NULL;
        RBE_ASSERT(activeScene->scenePath != NULL);
        pyh_run_python_file(activeScene->scenePath);
    }
}
