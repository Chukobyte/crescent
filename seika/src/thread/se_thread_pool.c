#include "se_thread_pool.h"

#include "../memory/se_mem.h"

// --- Thread Pool Worker --- //
static SEThreadPoolWork* tpool_work_create(SEThreadFunc func, void *arg) {
    SEThreadPoolWork* work;

    if (func == NULL) {
        return NULL;
    }

    work = SE_MEM_ALLOCATE_SIZE(sizeof(*work));
    work->func = func;
    work->arg = arg;
    work->next = NULL;
    return work;
}

static void tpool_work_destroy(SEThreadPoolWork* work) {
    if (work == NULL) {
        return;
    }
    SE_MEM_FREE(work);
}


static SEThreadPoolWork* tpool_work_get(SEThreadPool* tp) {
    SEThreadPoolWork* work;

    if (tp == NULL) {
        return NULL;
    }

    work = tp->workFirst;
    if (work == NULL) {
        return NULL;
    }

    if (work->next == NULL) {
        tp->workFirst = NULL;
        tp->workLast  = NULL;
    } else {
        tp->workFirst = work->next;
    }

    return work;
}

static void* tpool_worker(void *arg) {
    SEThreadPool* tp = arg;
    SEThreadPoolWork* work;

    while (true) {
        pthread_mutex_lock(&(tp->workMutex));

        while (tp->workFirst == NULL && !tp->shouldStop) {
            pthread_cond_wait(&(tp->workCond), &(tp->workMutex));
        }

        if (tp->shouldStop) {
            break;
        }

        work = tpool_work_get(tp);
        tp->workingCount++;
        pthread_mutex_unlock(&(tp->workMutex));

        if (work != NULL) {
            work->func(work->arg);
            tpool_work_destroy(work);
        }

        pthread_mutex_lock(&(tp->workMutex));
        tp->workingCount--;
        if (!tp->shouldStop && tp->workingCount == 0 && tp->workFirst == NULL) {
            pthread_cond_signal(&(tp->workingCond));
        }
        pthread_mutex_unlock(&(tp->workMutex));
    }

    tp->threadCount--;
    pthread_cond_signal(&(tp->workingCond));
    pthread_mutex_unlock(&(tp->workMutex));
    return NULL;
}


// --- Thread Pool --- //
SEThreadPool* se_tpool_create(size_t num) {
    SEThreadPool* tp;
    pthread_t thread;

    if (num == 0) {
        num = 2;
    }

    tp = SE_MEM_ALLOCATE_SIZE_ZERO(1, sizeof(*tp));
    tp->threadCount = num;

    pthread_mutex_init(&(tp->workMutex), NULL);
    pthread_cond_init(&(tp->workCond), NULL);
    pthread_cond_init(&(tp->workingCond), NULL);

    tp->workFirst = NULL;
    tp->workLast  = NULL;

    for (size_t i = 0; i < num; i++) {
        pthread_create(&thread, NULL, tpool_worker, tp);
        pthread_detach(thread);
    }

    return tp;
}

void se_tpool_destroy(SEThreadPool* tp) {
    SEThreadPoolWork* work;
    SEThreadPoolWork* work2;

    if (tp == NULL)
        return;

    pthread_mutex_lock(&(tp->workMutex));
    work = tp->workFirst;
    while (work != NULL) {
        work2 = work->next;
        tpool_work_destroy(work);
        work = work2;
    }
    tp->shouldStop = true;
    pthread_cond_broadcast(&(tp->workCond));
    pthread_mutex_unlock(&(tp->workMutex));

    se_tpool_wait(tp);

    pthread_mutex_destroy(&(tp->workMutex));
    pthread_cond_destroy(&(tp->workCond));
    pthread_cond_destroy(&(tp->workingCond));

    SE_MEM_FREE(tp);
}

bool se_tpool_add_work(SEThreadPool* tp, SEThreadFunc func, void* arg) {
    SEThreadPoolWork* work;

    if (tp == NULL) {
        return false;
    }

    work = tpool_work_create(func, arg);
    if (work == NULL) {
        return false;
    }

    pthread_mutex_lock(&(tp->workMutex));
    if (tp->workFirst == NULL) {
        tp->workFirst = work;
        tp->workLast  = tp->workFirst;
    } else {
        tp->workLast->next = work;
        tp->workLast       = work;
    }

    pthread_cond_broadcast(&(tp->workCond));
    pthread_mutex_unlock(&(tp->workMutex));

    return true;
}

void se_tpool_wait(SEThreadPool* tp) {
    if (tp == NULL) {
        return;
    }

    pthread_mutex_lock(&(tp->workMutex));
    while (true) {
        if ((!tp->shouldStop && tp->workingCount != 0) || (tp->shouldStop && tp->threadCount != 0)) {
            pthread_cond_wait(&(tp->workingCond), &(tp->workMutex));
        } else {
            break;
        }
    }
    pthread_mutex_unlock(&(tp->workMutex));
}
