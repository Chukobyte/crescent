#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "rbe_pthread.h"

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
bool tpool_add_work(ThreadPool* tp, ThreadFunc func, void* arg);
void tpool_wait(ThreadPool* tp);
