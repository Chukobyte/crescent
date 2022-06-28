#pragma once

#include <stdbool.h>

// Macros that are set up to run empty statements if missing a 'define'.

#ifndef DEBUG
#define RBE_DEBUG_MACRO(STATEMENT)                                                                                     \
    do {                                                                                                               \
        STATEMENT                                                                                                      \
    } while (false)
#else
#define RBE_DEBUG_MACRO(CONDITION, STATEMENT)                                                                          \
    do {                                                                                                               \
    } while (false)
#endif
