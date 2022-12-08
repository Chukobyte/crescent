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

//--- Subject ---//
SESubject* se_subject_new() {
    SESubject* subject = SE_MEM_ALLOCATE(SESubject);
    subject->observerCount = 0;
    return subject;
}

void se_subject_delete(SESubject* subject) {
    SE_MEM_FREE(subject);
}

bool se_subject_register_observer(SESubject* subject, SEObserver* observer) {
    SE_ASSERT(subject != NULL);
    SE_ASSERT(observer != NULL);
    SE_ASSERT_FMT(subject->observerCount + 1 < SE_MAX_OBSERVERS, "Reached max observer count, consider increasing 'SE_MAX_OBSERVERS'!");
    subject->observers[subject->observerCount++] = observer;
    return true;
}

bool se_subject_unregister_observer(SESubject* subject, SEObserver* observer) {
    SE_ARRAY_UTILS_REMOVE_ARRAY_ITEM(subject->observers, subject->observerCount, observer, NULL);
    return true;
}

void se_subject_notify_observers(SESubject* subject, SESubjectNotifyPayload* payload) {
    for (size_t i = 0; i < subject->observerCount; i++) {
        subject->observers[i]->on_notify(payload);
    }
}
