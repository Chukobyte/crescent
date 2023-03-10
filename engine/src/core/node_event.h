#pragma once

#include <stddef.h>

#include "ecs/entity/entity.h"

typedef struct NodeEventNotifyPayload {
    void* data;
} NodeEventNotifyPayload;

typedef void (*NodeEventObserverCallback)(void*, NodeEventNotifyPayload*); // (Event payload, observer data)
typedef void (*NodeEventObserverDataDeleteCallback)(void*);

// Will only create if it doesn't exist
void node_event_create_event(CreEntity entity, const char* eventId);
void node_event_subscribe_to_event(CreEntity entity, const char* eventId, CreEntity observerEntity, NodeEventObserverCallback observerCallback, void* observerData, NodeEventObserverDataDeleteCallback dataDeleteCallback);
void node_event_notify_observers(CreEntity entity, const char* eventId, NodeEventNotifyPayload* payload);
void node_event_destroy_all_entity_events_and_observers(CreEntity entity);
// Queries
size_t node_event_get_event_count(CreEntity entity);
size_t node_event_get_event_observer_count(CreEntity entity, const char* eventId);
size_t node_event_get_entity_observer_count(CreEntity entity);
