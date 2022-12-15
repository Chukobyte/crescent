#include "node_event.h"

#include "../seika/src/data_structures/se_hash_map_string.h"
#include "../seika/src/memory/se_mem.h"

#include "ecs/component/node_component.h"
#include "ecs/component/component.h"
#include "../../../seika/src/data_structures/se_array_utils.h"

#define MAX_EVENT_OBSERVER_ENTRY_COUNT 16

typedef struct NodeEventObserverEntry {
    size_t entryCount;
    NodeEventObserver* observers[MAX_EVENT_OBSERVER_ENTRY_COUNT];
} NodeEventObserverEntry;

typedef struct NodeEventDatabase {
    // Keeps tracks of an entity's events and observers
    SEStringHashMap* entityEventMaps[MAX_ENTITIES];
    // Keeps tracks of observers to events
    NodeEventObserverEntry nodeEventObserverEntry[MAX_ENTITIES];
} NodeEventDatabase;

NodeEventObserver* node_create_observer(Entity entity, NodeEvent* event, void* data);

void cre_node_event_on_entity_exit_scene(SESubjectNotifyPayload* payload);

static NodeEventDatabase eventDatabase = { .entityEventMaps = {NULL}, .nodeEventObserverEntry = {0} };

NodeEvent* node_event_create_event(Entity entity, const char* eventId) {
    if (eventDatabase.entityEventMaps[entity] == NULL) {
        eventDatabase.entityEventMaps[entity] = se_string_hash_map_create_default_capacity();
        NodeComponent* nodeComp = component_manager_get_component_unsafe(entity, ComponentDataIndex_NODE);
        if (nodeComp != NULL) {
            static SEObserver nodeEntityOnExitSceneObserver = { .on_notify = cre_node_event_on_entity_exit_scene };
            se_event_register_observer(&nodeComp->onSceneTreeExit, &nodeEntityOnExitSceneObserver);
        }
    }
    if (!se_string_hash_map_has(eventDatabase.entityEventMaps[entity], eventId)) {
        NodeEvent* event = SE_MEM_ALLOCATE(NodeEvent);
        event->entity = entity;
        event->observerCount = 0;
        se_string_hash_map_add(eventDatabase.entityEventMaps[entity], eventId, &event, sizeof(NodeEvent**));
    }
    NodeEvent* event = (NodeEvent*) *(NodeEvent**) se_string_hash_map_get(eventDatabase.entityEventMaps[entity], eventId);
    return event;
}

void node_event_subscribe_to_event(Entity entity, const char* eventId, Entity observerEntity) {
    NodeEvent* event = node_event_create_event(entity, eventId);
    event->observers[event->observerCount++] = node_create_observer(observerEntity, event, NULL);
}

void node_event_notify_observers(Entity entity, const char* eventId, NodeEventNotifyPayload* payload) {
    NodeEvent* event = node_event_create_event(entity, eventId);
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
        // TODO: Clean up data
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
            SE_MEM_FREE(nodeEvent);
        }
        se_string_hash_map_destroy(eventDatabase.entityEventMaps[entity]);
        eventDatabase.entityEventMaps[entity] = NULL;
    }
}

NodeEventObserver* node_create_observer(Entity entity, NodeEvent* event, void* data) {
    // TODO: Check for duplicates
    NodeEventObserver* eventObserver = SE_MEM_ALLOCATE(NodeEventObserver);
    eventObserver->entity = entity;
    eventObserver->data = data;
    eventObserver->event = event;
    // Get alias to shorten the expression
    NodeEventObserverEntry* observerEntry = &eventDatabase.nodeEventObserverEntry[entity];
    observerEntry->observers[observerEntry->entryCount++] = eventObserver;
    return eventObserver;
}

// Callbacks
void cre_node_event_on_entity_exit_scene(SESubjectNotifyPayload* payload) {
    const Entity entity = *(Entity*) payload->data;
    node_event_destroy_all_entity_events_and_observers(entity);
}
