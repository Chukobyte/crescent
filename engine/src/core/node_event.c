#include "node_event.h"

#include <string.h>

#include <seika/ecs/ecs.h>
#include <seika/data_structures/se_hash_map_string.h>
#include <seika/data_structures/se_array_utils.h>
#include <seika/memory/se_mem.h>
#include <seika/utils/se_string_util.h>
#include <seika/utils/se_assert.h>

#include "ecs/ecs_globals.h"
#include "ecs/component/node_component.h"
#include "ecs/component/component.h"

#define CRE_NODE_EVENT_MAX_OBSERVERS 8
#define MAX_EVENT_OBSERVER_ENTRY_COUNT 100

typedef struct NodeEventObserver {
    CreEntity entity;
    NodeEventObserverCallback callback;
    void* data; // Used to store misc data from the caller
    NodeEventObserverDataDeleteCallback dataDeleteCallback;
    struct NodeEvent* event;
} NodeEventObserver;

typedef struct NodeEvent {
    CreEntity entity;
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
    SEStringHashMap* entityEventMaps[CRE_MAX_ENTITIES];
    // Keeps tracks of observers to events
    NodeEventObserverEntry nodeEventObserverEntry[CRE_MAX_ENTITIES];
    // Booleans for if entity has registered to the 'on scene exit' callback (TODO: Should be done better...)
    bool hasEntityRegisteredOnSceneExitCallback[CRE_MAX_ENTITIES];
} NodeEventDatabase;

NodeEvent* node_event_create_event_internal(CreEntity entity, const char* eventId);
NodeEventObserver* node_create_observer_internal(CreEntity entity, NodeEvent* event, NodeEventObserverCallback observerCallback, void* observerData, NodeEventObserverDataDeleteCallback dataDeleteCallback);
void node_observer_free(NodeEventObserver* observer);
bool does_entity_have_observer_event_already(CreEntity observerEntity, NodeEvent* event);
void register_entity_to_on_scene_exit_callback(CreEntity entity);
void unregister_entity_to_on_scene_exit_callback(CreEntity entity);

void cre_node_event_on_entity_exit_scene(SESubjectNotifyPayload* payload);

static NodeEventDatabase eventDatabase = {
    .entityEventMaps = {NULL},
    .nodeEventObserverEntry = {{0}},
    .hasEntityRegisteredOnSceneExitCallback = {false}
};
static SEObserver nodeEntityOnExitSceneObserver = { .on_notify = cre_node_event_on_entity_exit_scene };

void node_event_create_event(CreEntity entity, const char* eventId) {
    node_event_create_event_internal(entity, eventId);
}

void node_event_subscribe_to_event(CreEntity entity, const char* eventId, CreEntity observerEntity, NodeEventObserverCallback observerCallback, void* observerData, NodeEventObserverDataDeleteCallback dataDeleteCallback) {
    NodeEvent* event = node_event_create_event_internal(entity, eventId);
    if (!does_entity_have_observer_event_already(observerEntity, event)) {
        event->observers[event->observerCount++] = node_create_observer_internal(observerEntity, event, observerCallback, observerData, dataDeleteCallback);
    }
}

void node_event_notify_observers(CreEntity entity, const char* eventId, NodeEventNotifyPayload* payload) {
    NodeEvent* event = node_event_create_event_internal(entity, eventId);
    // Copy observers in case they're deleted in a callback
    static NodeEventObserver* observersToNotify[CRE_NODE_EVENT_MAX_OBSERVERS];
    const size_t observerCount = event->observerCount;
    for (size_t i = 0; i < observerCount; i++) {
        observersToNotify[i] = event->observers[i];
    }
    // Invoke observer callbacks
    for (size_t i = 0; i < observerCount; i++) {
        observersToNotify[i]->callback(observersToNotify[i]->data, payload);
    }
}

void node_event_destroy_all_entity_events_and_observers(CreEntity entity) {
    // Remove all entity observers
    NodeEventObserverEntry* observerEntry = &eventDatabase.nodeEventObserverEntry[entity];
    for (size_t i = 0; i < observerEntry->entryCount; i++) {
        NodeEventObserver* nodeEventObserver = observerEntry->observers[i];
        if (nodeEventObserver == NULL || nodeEventObserver->event == NULL) {
            continue;
        }
        NodeEvent* nodeEvent = nodeEventObserver->event;
        SE_ARRAY_UTILS_REMOVE_ARRAY_ITEM(nodeEvent->observers, nodeEvent->observerCount, nodeEventObserver, NULL);
        node_observer_free(nodeEventObserver);
        observerEntry->observers[i] = NULL;
    }
    observerEntry->entryCount = 0;
    // Remove all entity events
    if (eventDatabase.entityEventMaps[entity] != NULL) {
        SE_STRING_HASH_MAP_FOR_EACH(eventDatabase.entityEventMaps[entity], iter) {
            StringHashMapNode* node = iter.pair;
            NodeEvent* nodeEvent = (NodeEvent*) *(NodeEvent**) node->value;
            // Remove all observers attached to event
            for (size_t i = 0; i < nodeEvent->observerCount; i++) {
                SE_ARRAY_UTILS_REMOVE_ARRAY_ITEM(
                    eventDatabase.nodeEventObserverEntry[nodeEvent->observers[i]->entity].observers,
                    eventDatabase.nodeEventObserverEntry[nodeEvent->observers[i]->entity].entryCount,
                    nodeEvent->observers[i],
                    NULL
                );
                node_observer_free(nodeEvent->observers[i]);
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
size_t node_event_get_event_count(CreEntity entity) {
    return eventDatabase.entityEventMaps[entity] != NULL ? eventDatabase.entityEventMaps[entity]->size : 0;
}

size_t node_event_get_event_observer_count(CreEntity entity, const char* eventId) {
    if (eventDatabase.entityEventMaps[entity] != NULL) {
        NodeEvent* event = node_event_create_event_internal(entity, eventId);
        return event->observerCount;
    }
    return 0;
}

size_t node_event_get_entity_observer_count(CreEntity entity) {
    return eventDatabase.nodeEventObserverEntry[entity].entryCount;
}

// Internal
NodeEvent* node_event_create_event_internal(CreEntity entity, const char* eventId) {
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

NodeEventObserver* node_create_observer_internal(CreEntity entity, NodeEvent* event, NodeEventObserverCallback observerCallback, void* observerData, NodeEventObserverDataDeleteCallback dataDeleteCallback) {
    NodeEventObserver* eventObserver = SE_MEM_ALLOCATE(NodeEventObserver);
    eventObserver->entity = entity;
    eventObserver->callback = observerCallback;
    eventObserver->data = observerData;
    eventObserver->dataDeleteCallback = dataDeleteCallback;
    eventObserver->event = event;
    // Get alias to shorten the expression
    NodeEventObserverEntry* observerEntry = &eventDatabase.nodeEventObserverEntry[entity];
    SE_ASSERT(observerEntry->entryCount + 1 < MAX_EVENT_OBSERVER_ENTRY_COUNT);
    observerEntry->observers[observerEntry->entryCount++] = eventObserver;
    register_entity_to_on_scene_exit_callback(entity);
    return eventObserver;
}

void node_observer_free(NodeEventObserver* observer) {
    if (observer->data != NULL && observer->dataDeleteCallback != NULL) {
        observer->dataDeleteCallback(observer->data);
    }
    SE_MEM_FREE(observer);
}

bool does_entity_have_observer_event_already(CreEntity observerEntity, NodeEvent* event) {
    for (size_t i = 0; i < eventDatabase.nodeEventObserverEntry[observerEntity].entryCount; i++) {
        const NodeEvent* observerEvent = eventDatabase.nodeEventObserverEntry[observerEntity].observers[i]->event;
        if (strcmp(observerEvent->id, event->id) == 0 && observerEvent->entity == event->entity) {
            return true;
        }
    }
    return false;
}

void register_entity_to_on_scene_exit_callback(CreEntity entity) {
    NodeComponent* nodeComp = NULL;
    if (!eventDatabase.hasEntityRegisteredOnSceneExitCallback[entity] && (nodeComp = ska_ecs_component_manager_get_component_unchecked(entity, NODE_COMPONENT_INDEX))) {
        se_event_register_observer(&nodeComp->onSceneTreeExit, &nodeEntityOnExitSceneObserver);
        eventDatabase.hasEntityRegisteredOnSceneExitCallback[entity] = true;
    }
}

void unregister_entity_to_on_scene_exit_callback(CreEntity entity) {
    NodeComponent* nodeComp = NULL;
    if (eventDatabase.hasEntityRegisteredOnSceneExitCallback[entity] && (nodeComp = ska_ecs_component_manager_get_component_unchecked(entity, NODE_COMPONENT_INDEX))) {
        se_event_unregister_observer(&nodeComp->onSceneTreeExit, &nodeEntityOnExitSceneObserver);
        eventDatabase.hasEntityRegisteredOnSceneExitCallback[entity] = false;
    }
}

// Callbacks
void cre_node_event_on_entity_exit_scene(SESubjectNotifyPayload* payload) {
    const CreEntity entity = *(CreEntity*) payload->data;
    node_event_destroy_all_entity_events_and_observers(entity);
}
