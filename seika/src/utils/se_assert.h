#include <assert.h>
#include <stdlib.h>

#include "se_macro_util.h"
#include "logger.h"

#define SE_ASSERT_FAIL_EXIT_CODE (-1)

#define SE_ASSERT(PREDICATE) \
SE_DEBUG_MACRO(              \
if (!(PREDICATE)) {      \
se_logger_print_err("Assertion failed in file '%s' on line %u!\n", __FILE__, __LINE__); \
exit(SE_ASSERT_FAIL_EXIT_CODE);                 \
}                                       \
)

#define SE_ASSERT_FMT(PREDICATE, FMT, ...) \
SE_DEBUG_MACRO(              \
if (!(PREDICATE)) {      \
se_logger_print_err("Assertion failed in file '%s' on line %u!", __FILE__, __LINE__); \
se_logger_print_err(FMT, ##__VA_ARGS__); \
exit(SE_ASSERT_FAIL_EXIT_CODE);                 \
}                                       \
)
