#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "se_pthread.h"

typedef void (*ThreadFunc)(void* arg);

typedef struct ThreadPoolWork {
    ThreadFunc func;
    void* arg;
    struct ThreadPoolWork* next;
} ThreadPoolWork;

typedef struct ThreadPool {
    ThreadPoolWork* workFirst;
    ThreadPoolWork* workLast;
    pthread_mutex_t workMutex;
    pthread_cond_t workCond;
    pthread_cond_t workingCond;
    size_t workingCount;
    size_t threadCount;
    bool shouldStop;
} ThreadPool;

ThreadPool* tpool_create(size_t num);
void tpool_destroy(ThreadPool* tp);
// Adds worker (or job) to the thread pool.
bool tpool_add_work(ThreadPool* tp, ThreadFunc func, void* arg);
// Blocking function that finishes once all work is completed.
void tpool_wait(ThreadPool* tp);
