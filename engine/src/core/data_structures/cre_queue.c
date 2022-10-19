#include "cre_queue.h"

#include "../memory/rbe_mem.h"

CREQueue* cre_queue_create(size_t capacity, uint32_t invalidValue) {
    CREQueue* queue = RBE_MEM_ALLOCATE(CREQueue);
    queue->capacity = capacity;
    queue->invalidValue = invalidValue;
    queue->size = 0;
    queue->front = 0;
    queue->rear = capacity - 1;
    queue->array = (uint32_t*) RBE_MEM_ALLOCATE_SIZE(queue->capacity * sizeof(uint32_t));
    return queue;
}

void cre_queue_destroy(CREQueue* queue) {
    RBE_MEM_FREE(queue->array);
    RBE_MEM_FREE(queue);
}

bool cre_queue_is_full(CREQueue* queue) {
    return queue->size == queue->capacity;
}

bool cre_queue_is_empty(CREQueue* queue) {
    return queue->size == 0;
}

void cre_queue_enqueue(CREQueue* queue, uint32_t item) {
    if (cre_queue_is_full(queue)) {
        return;
    }
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->array[queue->rear] = item;
    queue->size++;
}

uint32_t cre_queue_dequeue(CREQueue* queue) {
    if (cre_queue_is_empty(queue)) {
        return queue->invalidValue;
    }
    uint32_t value = queue->array[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;
    return value;
}

uint32_t cre_queue_front(CREQueue* queue) {
    if (cre_queue_is_empty(queue)) {
        return queue->invalidValue;
    }
    return queue->array[queue->front];
}

uint32_t cre_queue_rear(CREQueue* queue) {
    if (cre_queue_is_empty(queue)) {
        return queue->invalidValue;
    }
    return queue->array[queue->rear];
}
