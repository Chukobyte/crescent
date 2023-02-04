#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// TODO: If other types are needed, create a macro to simplify queues by type

// Unsigned int queue
typedef struct SEQueue {
    uint32_t front, rear, size, invalidValue;
    size_t capacity;
    uint32_t* array;
} SEQueue;

SEQueue* se_queue_create(size_t capacity, uint32_t invalidValue);
void se_queue_destroy(SEQueue* queue);
bool se_queue_is_full(SEQueue* queue);
bool se_queue_is_empty(SEQueue* queue);
void se_queue_enqueue(SEQueue* queue, uint32_t item);
uint32_t se_queue_dequeue(SEQueue* queue);
uint32_t se_queue_front(SEQueue* queue);
uint32_t se_queue_rear(SEQueue* queue);
