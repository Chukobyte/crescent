#pragma once

// Helper class for generalized use of static arrays

#define RBE_STATIC_ARRAY_CREATE(ARRAY_TYPE, ARRAY_SIZE, ARRAY_NAME) \
static ARRAY_TYPE ARRAY_NAME[ARRAY_SIZE];                           \
static size_t ARRAY_NAME ##_count = 0

#define RBE_STATIC_ARRAY_ADD(ARRAY_NAME, ARRAY_VALUE) \
ARRAY_NAME[ARRAY_NAME ##_count++] = ARRAY_VALUE

#define RBE_STATIC_ARRAY_REMOVE(ARRAY_NAME, ARRAY_VALUE, EMPTY_VALUE) \
{                                                                     \
const size_t ARRAY_NAME ##_count_ref = ARRAY_NAME ##_count;\
for (size_t i = 0; i < ARRAY_NAME ##_count_ref; i++) {       \
if (ARRAY_NAME[i] == ARRAY_VALUE) {                      \
ARRAY_NAME[i] = ARRAY_NAME[i + 1];                       \
ARRAY_NAME[i + 1] = EMPTY_VALUE;                         \
ARRAY_NAME ##_count--;                                   \
}                                                        \
if (ARRAY_NAME[i] == EMPTY_VALUE) {                      \
ARRAY_NAME[i] = ARRAY_NAME[i + 1];                       \
ARRAY_NAME[i + 1] = EMPTY_VALUE;                         \
}                                                        \
}                                                        \
}

#define RBE_STATIC_ARRAY_EMPTY(ARRAY_NAME) \
ARRAY_NAME ##_count = 0;
