#include "se_queue.h"

#include "../memory/se_mem.h"

SEQueue* se_queue_create(size_t capacity, uint32_t invalidValue) {
    SEQueue* queue = SE_MEM_ALLOCATE(SEQueue);
    queue->capacity = capacity;
    queue->invalidValue = invalidValue;
    queue->size = 0;
    queue->front = 0;
    queue->rear = capacity - 1;
    queue->array = (uint32_t*) SE_MEM_ALLOCATE_SIZE(queue->capacity * sizeof(uint32_t));
    return queue;
}

void se_queue_destroy(SEQueue* queue) {
    SE_MEM_FREE(queue->array);
    SE_MEM_FREE(queue);
}

bool se_queue_is_full(SEQueue* queue) {
    return queue->size == queue->capacity;
}

bool se_queue_is_empty(SEQueue* queue) {
    return queue->size == 0;
}

void se_queue_enqueue(SEQueue* queue, uint32_t item) {
    if (se_queue_is_full(queue)) {
        return;
    }
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->array[queue->rear] = item;
    queue->size++;
}

uint32_t se_queue_dequeue(SEQueue* queue) {
    if (se_queue_is_empty(queue)) {
        return queue->invalidValue;
    }
    uint32_t value = queue->array[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;
    return value;
}

uint32_t se_queue_front(SEQueue* queue) {
    if (se_queue_is_empty(queue)) {
        return queue->invalidValue;
    }
    return queue->array[queue->front];
}

uint32_t se_queue_rear(SEQueue* queue) {
    if (se_queue_is_empty(queue)) {
        return queue->invalidValue;
    }
    return queue->array[queue->rear];
}
