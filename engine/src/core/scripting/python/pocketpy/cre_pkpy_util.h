#pragma once

#include <pocketpy/pocketpy_c.h>

#define CRE_PKPY_MODULE_FUNCTION_LIMIT 8

typedef struct CrePPFunction {
    const char* signature;
    pkpy_CFunction function;
} CrePPFunction;

typedef struct CrePPModule {
    const char* name;
    size_t functionCount;
    CrePPFunction functions[CRE_PKPY_MODULE_FUNCTION_LIMIT];
} CrePPModule;

char* cre_pkpy_util_get_error_message(pkpy_vm* vm);
bool cre_pkpy_util_print_error_message(pkpy_vm* vm);
void cre_pkpy_util_create_module(pkpy_vm* vm, CrePPModule* module);
