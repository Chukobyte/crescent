#pragma once

#include <stddef.h>
#include <stdbool.h>

#define SE_MAX_OBSERVERS 8

typedef struct SESubjectNotifyPayload {
    void* data; // Primary data, be sure to cast properly
    int type; // Secondary data, can be used to filter things, an easier value to access, etc...
} SESubjectNotifyPayload;

typedef void (*SEObserverOnNotify)(SESubjectNotifyPayload*);

// An observer that can subscribe to a subject
typedef struct SEObserver {
    SEObserverOnNotify on_notify;
} SEObserver;

SEObserver* se_observer_new(SEObserverOnNotify onNotifyFunc);
void se_observer_delete(SEObserver* observer);

// A subscribable event
typedef struct SEEvent {
    size_t observerCount;
    SEObserver* observers[SE_MAX_OBSERVERS];
} SEEvent;

SEEvent* se_event_new();
void se_event_delete(SEEvent* event);
bool se_event_register_observer(SEEvent* event, SEObserver* observer);
bool se_event_unregister_observer(SEEvent* event, SEObserver* observer);
void se_event_notify_observers(SEEvent* event, SESubjectNotifyPayload* payload);
