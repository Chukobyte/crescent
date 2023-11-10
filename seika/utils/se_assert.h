#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include <stdlib.h>

#include "se_macro_util.h"
#include "se_platform.h"
#include "logger.h"

#define SE_ASSERT_FAIL_EXIT_CODE (-1)

#define SE_ASSERT(PREDICATE) \
SE_DEBUG_MACRO(                                                                         \
if (!(PREDICATE)) {                                                                     \
se_logger_print_err("Assertion failed in file '%s' on line %u!\n", __FILE__, __LINE__); \
if (se_platform_is_debugger_attached()) { SE_TRIGGER_BREAKPOINT; }                      \
exit(SE_ASSERT_FAIL_EXIT_CODE);                                                         \
}                                                                                       \
)

#define SE_ASSERT_FMT(PREDICATE, FMT, ...)                                            \
SE_DEBUG_MACRO(                                                                       \
if (!(PREDICATE)) {                                                                   \
se_logger_print_err("Assertion failed in file '%s' on line %u!", __FILE__, __LINE__); \
se_logger_print_err(FMT, ##__VA_ARGS__);                                              \
if (se_platform_is_debugger_attached()) { SE_TRIGGER_BREAKPOINT; }                    \
exit(SE_ASSERT_FAIL_EXIT_CODE);                                                       \
}                                                                                     \
)

// Non-fatal asserts
#define SE_ENSURE(PREDICATE)                                                         \
SE_DEBUG_MACRO(                                                                      \
if (!(PREDICATE)) {                                                                  \
static bool hasEnsureTriggered = false;                                              \
if (!hasEnsureTriggered) {                                                           \
se_logger_print_err("Ensure failed in file '%s' on line %u!\n", __FILE__, __LINE__); \
if (se_platform_is_debugger_attached()) { SE_TRIGGER_BREAKPOINT; }                   \
hasEnsureTriggered = true;                                                           \
}                                                                                    \
}                                                                                    \
)

#define SE_ENSURE_FMT(PREDICATE, FMT, ...)                                         \
SE_DEBUG_MACRO(                                                                    \
if (!(PREDICATE)) {                                                                \
static bool hasEnsureTriggered = false;                                            \
if (!hasEnsureTriggered) {                                                         \
se_logger_print_err("Ensure failed in file '%s' on line %u!", __FILE__, __LINE__); \
se_logger_print_err(FMT, ##__VA_ARGS__);                                           \
if (se_platform_is_debugger_attached()) { SE_TRIGGER_BREAKPOINT; }                 \
hasEnsureTriggered = true;                                                         \
}                                                                                  \
}                                                                                  \
)

#define SE_ENSURE_ALWAYS(PREDICATE)                                                  \
SE_DEBUG_MACRO(                                                                      \
if (!(PREDICATE)) {                                                                  \
se_logger_print_err("Ensure failed in file '%s' on line %u!\n", __FILE__, __LINE__); \
if (se_platform_is_debugger_attached()) { SE_TRIGGER_BREAKPOINT; }                   \
}                                                                                    \
)

#define SE_ENSURE_ALWAYS_FMT(PREDICATE, FMT, ...)                                  \
SE_DEBUG_MACRO(                                                                    \
if (!(PREDICATE)) {                                                                \
se_logger_print_err("Ensure failed in file '%s' on line %u!", __FILE__, __LINE__); \
se_logger_print_err(FMT, ##__VA_ARGS__);                                           \
if (se_platform_is_debugger_attached()) { SE_TRIGGER_BREAKPOINT; }                 \
}                                                                                  \
)

#ifdef __cplusplus
}
#endif
