#include "node_event.h"

#include <string.h>

#include "../seika/src/data_structures/se_hash_map_string.h"
#include "../seika/src/data_structures/se_array_utils.h"
#include "../seika/src/utils/se_string_util.h"
#include "../seika/src/memory/se_mem.h"

#include "ecs/component/node_component.h"
#include "ecs/component/component.h"

#define CRE_NODE_EVENT_MAX_OBSERVERS 8
#define MAX_EVENT_OBSERVER_ENTRY_COUNT 16

typedef struct NodeEventObserver {
    Entity entity;
    NodeEventObserverCallback callback;
    void* data; // Used to store misc data from the caller
    struct NodeEvent* event;
} NodeEventObserver;

typedef struct NodeEvent {
    Entity entity;
    char* id;
    size_t observerCount;
    NodeEventObserver* observers[CRE_NODE_EVENT_MAX_OBSERVERS];
} NodeEvent;

typedef struct NodeEventObserverEntry {
    size_t entryCount;
    NodeEventObserver* observers[MAX_EVENT_OBSERVER_ENTRY_COUNT];
} NodeEventObserverEntry;

typedef struct NodeEventDatabase {
    // Keeps tracks of an entity's events and observers
    SEStringHashMap* entityEventMaps[MAX_ENTITIES];
    // Keeps tracks of observers to events
    NodeEventObserverEntry nodeEventObserverEntry[MAX_ENTITIES];
    // Booleans for if entity has registered to the 'on scene exit' callback (TODO: Should be done better...)
    bool hasEntityRegisteredOnSceneExitCallback[MAX_ENTITIES];
} NodeEventDatabase;

NodeEvent* node_event_create_event_internal(Entity entity, const char* eventId);
NodeEventObserver* node_create_observer_internal(Entity entity, NodeEvent* event, NodeEventObserverCallback observerCallback, void* observerData);
bool does_entity_have_observer_event_already(Entity observerEntity, NodeEvent* event);
void register_entity_to_on_scene_exit_callback(Entity entity);
void unregister_entity_to_on_scene_exit_callback(Entity entity);

void cre_node_event_on_entity_exit_scene(SESubjectNotifyPayload* payload);

static NodeEventDatabase eventDatabase = {
    .entityEventMaps = {NULL},
    .nodeEventObserverEntry = {0},
    .hasEntityRegisteredOnSceneExitCallback = {false}
};
static SEObserver nodeEntityOnExitSceneObserver = { .on_notify = cre_node_event_on_entity_exit_scene };

void node_event_create_event(Entity entity, const char* eventId) {
    node_event_create_event_internal(entity, eventId);
}

void node_event_subscribe_to_event(Entity entity, const char* eventId, Entity observerEntity, NodeEventObserverCallback observerCallback, void* observerData) {
    NodeEvent* event = node_event_create_event_internal(entity, eventId);
    if (!does_entity_have_observer_event_already(observerEntity, event)) {
        event->observers[event->observerCount++] = node_create_observer_internal(observerEntity, event, observerCallback, observerData);
    }
}

void node_event_notify_observers(Entity entity, const char* eventId, NodeEventNotifyPayload* payload) {
    NodeEvent* event = node_event_create_event_internal(entity, eventId);
    for (size_t i = 0; i < event->observerCount; i++) {
        event->observers[i]->callback(event->observers[i]->data, payload);
    }
}

void node_event_destroy_all_entity_events_and_observers(Entity entity) {
    // Remove all entity observers
    for (size_t i = 0; i < eventDatabase.nodeEventObserverEntry[entity].entryCount; i++) {
        NodeEventObserver* nodeEventObserver = eventDatabase.nodeEventObserverEntry[entity].observers[i];
        NodeEvent* nodeEvent = nodeEventObserver->event;
        SE_ARRAY_UTILS_REMOVE_ARRAY_ITEM(nodeEvent->observers, nodeEvent->observerCount, nodeEventObserver, NULL);
        SE_MEM_FREE(nodeEventObserver);
    }
    eventDatabase.nodeEventObserverEntry[entity].entryCount = 0;
    // Remove all entity events
    if (eventDatabase.entityEventMaps[entity] != NULL) {
        SE_STRING_HASH_MAP_FOR_EACH(eventDatabase.entityEventMaps[entity], iter) {
            StringHashMapNode* node = iter.pair;
            NodeEvent* nodeEvent = (NodeEvent*) *(NodeEvent**) node->value;
            // Remove all observers attached to event
            for (size_t i = 0; i < nodeEvent->observerCount; i++) {
                eventDatabase.nodeEventObserverEntry[nodeEvent->observers[i]->entity].observers;
                SE_ARRAY_UTILS_REMOVE_ARRAY_ITEM(
                    eventDatabase.nodeEventObserverEntry[nodeEvent->observers[i]->entity].observers,
                    eventDatabase.nodeEventObserverEntry[nodeEvent->observers[i]->entity].entryCount,
                    nodeEvent->observers[i],
                    NULL
                );
            }
            // Delete event
            SE_MEM_FREE(nodeEvent->id);
            SE_MEM_FREE(nodeEvent);
        }
        se_string_hash_map_destroy(eventDatabase.entityEventMaps[entity]);
        eventDatabase.entityEventMaps[entity] = NULL;
    }
    unregister_entity_to_on_scene_exit_callback(entity);
    eventDatabase.hasEntityRegisteredOnSceneExitCallback[entity] = false;
}

// Queries
size_t node_event_get_event_count(Entity entity) {
    return eventDatabase.entityEventMaps[entity] != NULL ? eventDatabase.entityEventMaps[entity]->size : 0;
}

size_t node_event_get_event_observer_count(Entity entity, const char* eventId) {
    if (eventDatabase.entityEventMaps[entity] != NULL) {
        NodeEvent* event = node_event_create_event_internal(entity, eventId);
        return event->observerCount;
    }
    return 0;
}

size_t node_event_get_entity_observer_count(Entity entity) {
    return eventDatabase.nodeEventObserverEntry[entity].entryCount;
}

// Internal
NodeEvent* node_event_create_event_internal(Entity entity, const char* eventId) {
    if (eventDatabase.entityEventMaps[entity] == NULL) {
        eventDatabase.entityEventMaps[entity] = se_string_hash_map_create_default_capacity();
        register_entity_to_on_scene_exit_callback(entity);
    }
    if (!se_string_hash_map_has(eventDatabase.entityEventMaps[entity], eventId)) {
        NodeEvent* event = SE_MEM_ALLOCATE(NodeEvent);
        event->entity = entity;
        event->id = se_strdup(eventId);
        event->observerCount = 0;
        se_string_hash_map_add(eventDatabase.entityEventMaps[entity], eventId, &event, sizeof(NodeEvent**));
    }
    NodeEvent* event = (NodeEvent*) *(NodeEvent**) se_string_hash_map_get(eventDatabase.entityEventMaps[entity], eventId);
    return event;
}

NodeEventObserver* node_create_observer_internal(Entity entity, NodeEvent* event, NodeEventObserverCallback observerCallback, void* observerData) {
    // TODO: Check for duplicates
    NodeEventObserver* eventObserver = SE_MEM_ALLOCATE(NodeEventObserver);
    eventObserver->entity = entity;
    eventObserver->callback = observerCallback;
    eventObserver->data = observerData;
    eventObserver->event = event;
    // Get alias to shorten the expression
    NodeEventObserverEntry* observerEntry = &eventDatabase.nodeEventObserverEntry[entity];
    observerEntry->observers[observerEntry->entryCount++] = eventObserver;
    register_entity_to_on_scene_exit_callback(entity);
    return eventObserver;
}

bool does_entity_have_observer_event_already(Entity observerEntity, NodeEvent* event) {
    for (size_t i = 0; i < eventDatabase.nodeEventObserverEntry[observerEntity].entryCount; i++) {
        const NodeEvent* observerEvent = eventDatabase.nodeEventObserverEntry[observerEntity].observers[i]->event;
        if (strcmp(observerEvent->id, event->id) == 0 && observerEvent->entity == event->entity) {
            return true;
        }
    }
    return false;
}

void register_entity_to_on_scene_exit_callback(Entity entity) {
    NodeComponent* nodeComp = NULL;
    if (!eventDatabase.hasEntityRegisteredOnSceneExitCallback[entity] && (nodeComp = component_manager_get_component_unsafe(entity, ComponentDataIndex_NODE))) {
        se_event_register_observer(&nodeComp->onSceneTreeExit, &nodeEntityOnExitSceneObserver);
        eventDatabase.hasEntityRegisteredOnSceneExitCallback[entity] = true;
    }
}

void unregister_entity_to_on_scene_exit_callback(Entity entity) {
    NodeComponent* nodeComp = NULL;
    if (eventDatabase.hasEntityRegisteredOnSceneExitCallback[entity] && (nodeComp = component_manager_get_component_unsafe(entity, ComponentDataIndex_NODE))) {
        se_event_unregister_observer(&nodeComp->onSceneTreeExit, &nodeEntityOnExitSceneObserver);
        eventDatabase.hasEntityRegisteredOnSceneExitCallback[entity] = false;
    }
}

// Callbacks
void cre_node_event_on_entity_exit_scene(SESubjectNotifyPayload* payload) {
    const Entity entity = *(Entity*) payload->data;
    node_event_destroy_all_entity_events_and_observers(entity);
}
