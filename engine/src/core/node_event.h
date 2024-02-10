#pragma once

#include <stddef.h>

#include <seika/ecs/entity.h>

typedef struct NodeEventNotifyPayload {
    void* data;
} NodeEventNotifyPayload;

typedef void (*NodeEventObserverCallback)(void*, NodeEventNotifyPayload*); // (Event payload, observer data)
typedef void (*NodeEventObserverDataDeleteCallback)(void*);

// Will only create if it doesn't exist
void node_event_create_event(SkaEntity entity, const char* eventId);
void node_event_subscribe_to_event(SkaEntity entity, const char* eventId, SkaEntity observerEntity, NodeEventObserverCallback observerCallback, void* observerData, NodeEventObserverDataDeleteCallback dataDeleteCallback);
void node_event_notify_observers(SkaEntity entity, const char* eventId, NodeEventNotifyPayload* payload);
void node_event_destroy_all_entity_events_and_observers(SkaEntity entity);
// Queries
size_t node_event_get_event_count(SkaEntity entity);
size_t node_event_get_event_observer_count(SkaEntity entity, const char* eventId);
size_t node_event_get_entity_observer_count(SkaEntity entity);
