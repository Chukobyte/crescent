#include "observer.h"

#include "../memory/se_mem.h"
#include "../data_structures/se_array_utils.h"
#include "se_assert.h"

//--- Observer ---//
SEObserver* se_observer_new(SEObserverOnNotify onNotifyFunc) {
    SE_ASSERT(onNotifyFunc != NULL);
    SEObserver* observer = SE_MEM_ALLOCATE(SEObserver);
    observer->on_notify = onNotifyFunc;
    return observer;
}

void se_observer_delete(SEObserver* observer) {
    SE_MEM_FREE(observer);
}

//--- Event ---//
SEEvent* se_event_new() {
    SEEvent* event = SE_MEM_ALLOCATE(SEEvent);
    event->observerCount = 0;
    return event;
}

void se_event_delete(SEEvent* event) {
    SE_MEM_FREE(event);
}

bool se_event_register_observer(SEEvent* event, SEObserver* observer) {
    SE_ASSERT(event != NULL);
    SE_ASSERT(observer != NULL);
    SE_ASSERT_FMT(event->observerCount + 1 < SE_MAX_OBSERVERS, "Reached max observer count, consider increasing 'SE_MAX_OBSERVERS'!");
    event->observers[event->observerCount++] = observer;
    return true;
}

bool se_event_unregister_observer(SEEvent* event, SEObserver* observer) {
    SE_ARRAY_UTILS_REMOVE_ARRAY_ITEM(event->observers, event->observerCount, observer, NULL);
    return true;
}

void se_event_notify_observers(SEEvent* event, SESubjectNotifyPayload* payload) {
    for (size_t i = 0; i < event->observerCount; i++) {
        event->observers[i]->on_notify(payload);
    }
}
