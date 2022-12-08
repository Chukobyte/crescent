#pragma once

#include <stddef.h>
#include <stdbool.h>

#define SE_MAX_OBSERVERS 8

typedef struct SESubjectNotifyPayload {
    void* data;
    int type;
} SESubjectNotifyPayload;

// Observer
typedef void (*SEObserverOnNotify)(SESubjectNotifyPayload*);

typedef struct SEObserver {
    SEObserverOnNotify on_notify;
} SEObserver;

SEObserver* se_observer_new(SEObserverOnNotify onNotifyFunc);
void se_observer_delete(SEObserver* observer);

// Subject
typedef struct SESubject {
    size_t observerCount;
    SEObserver* observers[SE_MAX_OBSERVERS];
} SESubject;

SESubject* se_subject_new();
void se_subject_delete(SESubject* subject);
bool se_subject_register_observer(SESubject* subject, SEObserver* observer);
bool se_subject_unregister_observer(SESubject* subject, SEObserver* observer);
void se_subject_notify_observers(SESubject* subject, SESubjectNotifyPayload* payload);
