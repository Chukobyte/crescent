#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "se_pthread.h"

typedef void (*ThreadFunc)(void* arg);

typedef struct SEThreadPoolWork {
    ThreadFunc func;
    void* arg;
    struct SEThreadPoolWork* next;
} SEThreadPoolWork;

typedef struct SEThreadPool {
    SEThreadPoolWork* workFirst;
    SEThreadPoolWork* workLast;
    pthread_mutex_t workMutex;
    pthread_cond_t workCond;
    pthread_cond_t workingCond;
    size_t workingCount;
    size_t threadCount;
    bool shouldStop;
} SEThreadPool;

SEThreadPool* se_tpool_create(size_t num);
void se_tpool_destroy(SEThreadPool* tp);
// Adds worker (or job) to the thread pool.
bool se_tpool_add_work(SEThreadPool* tp, ThreadFunc func, void* arg);
// Blocking function that finishes once all work is completed.
void se_tpool_wait(SEThreadPool* tp);
