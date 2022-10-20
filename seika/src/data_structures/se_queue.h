#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// TODO: If other types are needed, create a macro to simplify queues by type

// Unsigned int queue
typedef struct CREQueue {
    uint32_t front, rear, size, invalidValue;
    size_t capacity;
    uint32_t* array;
} CREQueue;

CREQueue* se_queue_create(size_t capacity, uint32_t invalidValue);
void se_queue_destroy(CREQueue* queue);
bool se_queue_is_full(CREQueue* queue);
bool se_queue_is_empty(CREQueue* queue);
void se_queue_enqueue(CREQueue* queue, uint32_t item);
uint32_t se_queue_dequeue(CREQueue* queue);
uint32_t se_queue_front(CREQueue* queue);
uint32_t se_queue_rear(CREQueue* queue);
