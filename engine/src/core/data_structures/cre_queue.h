#pragma once

#include <stdint.h>
#include <stdbool.h>

// Unsigned int queue, can create other types if needed...
typedef struct CREQueue {
    uint32_t front, rear, size, invalidValue;
    size_t capacity;
    uint32_t* array;
} CREQueue;

CREQueue* cre_queue_create(size_t capacity, uint32_t invalidValue);
void cre_queue_destroy(CREQueue* queue);
bool cre_queue_is_full(CREQueue* queue);
bool cre_queue_is_empty(CREQueue* queue);
void cre_queue_enqueue(CREQueue* queue, uint32_t item);
uint32_t cre_queue_dequeue(CREQueue* queue);
uint32_t cre_queue_front(CREQueue* queue);
uint32_t cre_queue_rear(CREQueue* queue);
