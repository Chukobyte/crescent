#pragma once

#include "ecs/entity/entity.h"

typedef struct NodeEventNotifyPayload {
    void* data;
} NodeEventNotifyPayload;

typedef void (*NodeEventObserverCallback)(void*, NodeEventNotifyPayload*); // (Event payload, observer data)

// Will only create if it doesn't exist
void node_event_create_event(Entity entity, const char* eventId);
void node_event_subscribe_to_event(Entity entity, const char* eventId, Entity observerEntity, NodeEventObserverCallback observerCallback, void* observerData);
void node_event_notify_observers(Entity entity, const char* eventId, NodeEventNotifyPayload* payload);
void node_event_destroy_all_entity_events_and_observers(Entity entity);
// Queries
size_t node_event_get_event_count(Entity entity);
size_t node_event_get_event_observer_count(Entity entity, const char* eventId);
size_t node_event_get_entity_observer_count(Entity entity);
