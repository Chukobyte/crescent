#pragma once

#include <pocketpy/pocketpy_c.h>

typedef struct CrePPFunction {
    const char* signature;
    pkpy_CFunction function;
} CrePPFunction;

typedef struct CrePPModule {
    const char* name;
    size_t functionCount;
    CrePPFunction functions[8];
} CrePPModule;

char* cre_py_pp_util_get_error_message(pkpy_vm* vm);
bool cre_py_pp_util_print_error_message(pkpy_vm* vm);
void cre_py_pp_util_create_module(pkpy_vm* vm, CrePPModule* module);
