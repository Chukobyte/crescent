#include <assert.h>

#include "rbe_macro_util.h"
#include "logger.h"

#define RBE_ASSERT_FAIL_EXIT_CODE -1

#define RBE_ASSERT(PREDICATE) \
RBE_DEBUG_MACRO(              \
if (!(PREDICATE)) {      \
rbe_logger_print_err("Assertion failed in file '%s' on line %u!\n", __FILE__, __LINE__); \
exit(RBE_ASSERT_FAIL_EXIT_CODE);                 \
}                                       \
)

#define RBE_ASSERT_FMT(PREDICATE, FMT, ...) \
RBE_DEBUG_MACRO(              \
if (!(PREDICATE)) {      \
rbe_logger_print_err("Assertion failed in file '%s' on line %u!", __FILE__, __LINE__); \
rbe_logger_print_err(FMT, ##__VA_ARGS__); \
exit(RBE_ASSERT_FAIL_EXIT_CODE);                 \
}                                       \
)
