#pragma once

#include <pocketpy/pocketpy_c.h>

#include <seika/utils/se_string_util.h>
#include <seika/utils/se_assert.h>

#define CRE_PKPY_MODULE_FUNCTION_LIMIT 256

typedef struct CrePPFunction {
    const char* signature;
    pkpy_CFunction function;
} CrePPFunction;

typedef struct CrePPModule {
    const char* name;
    CrePPFunction functions[CRE_PKPY_MODULE_FUNCTION_LIMIT];
} CrePPModule;

char* cre_pkpy_util_get_error_message(pkpy_vm* vm);
bool cre_pkpy_util_print_error_message(pkpy_vm* vm);
void cre_pkpy_util_create_module(pkpy_vm* vm, CrePPModule* module);
void cre_pkpy_util_create_from_string(pkpy_vm* vm, const char* moduleName, const char* moduleSource);

static inline const char* cre_pkpy_util_convert_pkpy_CString(const pkpy_CString* pkpyString) {
#define CONVERTED_STRING_BUFFER_SIZE 2048
    SE_ASSERT_FMT(pkpyString->size <= CONVERTED_STRING_BUFFER_SIZE, "pkpy string is larger than converted string buffer size, consider raising 'CONVERTED_STRING_BUFFER_SIZE'!");
    static char convertedStringBuffer[CONVERTED_STRING_BUFFER_SIZE];
    se_str_trim_by_size(pkpyString->data, convertedStringBuffer, pkpyString->size);
    return convertedStringBuffer;
#undef CONVERTED_STRING_BUFFER_SIZE
}
