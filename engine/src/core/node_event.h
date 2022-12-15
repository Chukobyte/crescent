#pragma once

#include "ecs/entity/entity.h"

#define CRE_NODE_EVENT_MAX_OBSERVERS 8

typedef struct NodeEventObserver {
    Entity entity;
    void* data;
    struct NodeEvent* event;
} NodeEventObserver;

typedef struct NodeEvent {
    Entity entity;
    size_t observerCount;
    NodeEventObserver* observers[CRE_NODE_EVENT_MAX_OBSERVERS];
} NodeEvent;

typedef struct NodeEventNotifyPayload {
    void* data;
} NodeEventNotifyPayload;

// Will only create if it doesn't exist
NodeEvent* node_event_create_event(Entity entity, const char* eventId);
void node_event_subscribe_to_event(Entity entity, const char* eventId, Entity observerEntity);
void node_event_notify_observers(Entity entity, const char* eventId, NodeEventNotifyPayload* payload);
void node_event_destroy_all_entity_events_and_observers(Entity entity);
