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

// A subscribable subject
typedef struct SESubject {
    size_t observerCount;
    SEObserver* observers[SE_MAX_OBSERVERS];
} SESubject;

SESubject* se_subject_new();
void se_subject_delete(SESubject* subject);
bool se_subject_register_observer(SESubject* subject, SEObserver* observer);
bool se_subject_unregister_observer(SESubject* subject, SEObserver* observer);
void se_subject_notify_observers(SESubject* subject, SESubjectNotifyPayload* payload);
