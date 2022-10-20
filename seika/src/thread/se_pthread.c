#include "se_pthread.h"

#ifdef _WIN32
#include "../memory/se_mem.h"

typedef struct {
    void* (*start_routine)(void*);
    void* start_arg;
} win_thread_start_t;

static DWORD WINAPI win_thread_start(void* arg);
static DWORD timespec_to_ms(const struct timespec *abstime);

// --- General --- //
int pthread_create(pthread_t* thread, pthread_attr_t* attr, void* (*start_routine)(void*), void* arg) {
    win_thread_start_t* data;

    (void) attr;

    if (thread == NULL || start_routine == NULL) {
        return 1; // ERROR
    }

    data = malloc(sizeof(*data));
    data->start_routine = start_routine;
    data->start_arg = arg;

    *thread = CreateThread(NULL, 0, win_thread_start, data, 0, NULL);
    if (*thread == NULL) {
        return 1; // ERROR
    }
    return 0; // SUCCESS
}

int pthread_join(pthread_t thread, void** value_ptr) {
    (void) value_ptr;
    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);
    return 0; // SUCCESS
}

int pthread_detach(pthread_t thread) {
    // TODO: Process return value
    CloseHandle(thread);
    return 0; // SUCCESS
}

int pthread_self() {
    return (int) GetCurrentThreadId();
}

// --- Mutex --- //
int pthread_mutex_init(pthread_mutex_t *mutex, pthread_mutexattr_t *attr) {
    (void)attr;

    if (mutex == NULL) {
        return 1; // ERROR
    }

    InitializeCriticalSection(mutex);
    return 0; // SUCCESS
}

int pthread_mutex_destroy(pthread_mutex_t *mutex) {
    if (mutex == NULL) {
        return 1; // ERROR
    }
    DeleteCriticalSection(mutex);
    return 0; // SUCCESS
}

int pthread_mutex_lock(pthread_mutex_t *mutex) {
    if (mutex == NULL) {
        return 1; // ERROR
    }
    EnterCriticalSection(mutex);
    return 0; // SUCCESS
}

int pthread_mutex_unlock(pthread_mutex_t *mutex) {
    if (mutex == NULL) {
        return 1; // ERROR
    }
    LeaveCriticalSection(mutex);
    return 0; // SUCCESS
}


// --- Conditionals --- //
int pthread_cond_init(pthread_cond_t *cond, pthread_condattr_t *attr) {
    (void) attr;
    if (cond == NULL) {
        return 1; // ERROR
    }
    InitializeConditionVariable(cond);
    return 0; // SUCCESS
}

int pthread_cond_destroy(pthread_cond_t* cond) {
    // Windows does not have a way to destroy conditionals
    (void) cond;
    return 0; // SUCCESS
}

int pthread_cond_wait(pthread_cond_t* cond, pthread_mutex_t *mutex) {
    if (cond == NULL || mutex == NULL) {
        return 1; // ERROR
    }
    return pthread_cond_timedwait(cond, mutex, NULL);
}

int pthread_cond_timedwait(pthread_cond_t* cond, pthread_mutex_t *mutex, const struct timespec *abstime) {
    if (cond == NULL || mutex == NULL) {
        return 1; // ERROR
    }
    if (!SleepConditionVariableCS(cond, mutex, timespec_to_ms(abstime))) {
        return 1; // ERROR
    }
    return 0; // SUCCESS
}

int pthread_cond_signal(pthread_cond_t* cond) {
    if (cond == NULL) {
        return 1; // ERROR
    }
    WakeConditionVariable(cond);
    return 0; // SUCCESS
}

int pthread_cond_broadcast(pthread_cond_t* cond) {
    if (cond == NULL) {
        return 1; // ERROR
    }
    WakeAllConditionVariable(cond);
    return 0; // SUCCESS
}

// --- Read Write Locks --- //
int pthread_rwlock_init(pthread_rwlock_t* rwlock, const pthread_rwlockattr_t* attr) {
    (void) attr;
    if (rwlock == NULL) {
        return 1;
    }
    InitializeSRWLock(&(rwlock->lock));
    rwlock->exclusive = false;
    return 0;
}

int pthread_rwlock_destroy(pthread_rwlock_t* rwlock) {
    (void) rwlock;
    return 0; // SUCCESS
}

int pthread_rwlock_rdlock(pthread_rwlock_t* rwlock) {
    if (rwlock == NULL) {
        return 1; // ERROR
    }
    AcquireSRWLockShared(&(rwlock->lock));
    return 0; // SUCCESS
}

int pthread_rwlock_tryrdlock(pthread_rwlock_t* rwlock) {
    if (rwlock == NULL) {
        return 1; // ERROR
    }
    return !TryAcquireSRWLockShared(&(rwlock->lock));
}

int pthread_rwlock_wrlock(pthread_rwlock_t* rwlock) {
    if (rwlock == NULL) {
        return 1; // ERROR
    }
    AcquireSRWLockExclusive(&(rwlock->lock));
    rwlock->exclusive = true;
    return 0; // SUCCESS
}

int pthread_rwlock_trywrlock(pthread_rwlock_t* rwlock) {
    BOOLEAN ret;

    if (rwlock == NULL) {
        return 1; // ERROR
    }

    ret = TryAcquireSRWLockExclusive(&(rwlock->lock));
    if (ret) {
        rwlock->exclusive = true;
    }
    return ret;
}

int pthread_rwlock_unlock(pthread_rwlock_t* rwlock) {
    if (rwlock == NULL) {
        return 1; // ERROR
    }

    if (rwlock->exclusive) {
        rwlock->exclusive = false;
        ReleaseSRWLockExclusive(&(rwlock->lock));
    } else {
        ReleaseSRWLockShared(&(rwlock->lock));
    }
    return 0; // SUCCESS
}


// --- MISC --- //
static DWORD WINAPI win_thread_start(void* arg) {
    win_thread_start_t* data = arg;
    void* (*start_routine)(void*) = data->start_routine;
    void* start_arg = data->start_arg;

    SE_MEM_FREE(data);

    start_routine(start_arg);
    return 0; // ERROR_SUCCESS
}

static DWORD timespec_to_ms(const struct timespec *abstime) {
    DWORD t;

    if (abstime == NULL) {
        return INFINITE;
    }

    t = ((abstime->tv_sec - time(NULL)) * 1000) + (abstime->tv_nsec / 1000000);
//    if (t < 0) {
//        t = 1;
//    }
    return t;
}

//#ifdef _WIN32
unsigned int pcthread_get_num_procs() {
    SYSTEM_INFO sysinfo;

    GetSystemInfo(&sysinfo);
    return sysinfo.dwNumberOfProcessors;
}

#else

#include <unistd.h>
unsigned int pcthread_get_num_procs() {
    return (unsigned int)sysconf(_SC_NPROCESSORS_ONLN);
}
#endif

void ms_to_timespec(struct timespec *ts, unsigned int ms) {
    if (ts == NULL) {
        return;
    }
    ts->tv_sec = ((long) ms / 1000) + time(NULL);
    ts->tv_nsec = ((long) ms % 1000) * 1000000;
}
