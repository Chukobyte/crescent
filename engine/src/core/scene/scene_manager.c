#include "scene_manager.h"

#include "../scripting/script_context.h"

Entity entitiesQueuedForCreation[MAX_ENTITIES];
size_t entitiesQueuedForCreationSize = 0;
Entity entitiesQueuedForDeletion[MAX_ENTITIES];
size_t entitiesQueuedForDeletionSize = 0;

RBEScriptContext* sceneManagerScriptContext = NULL;

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
