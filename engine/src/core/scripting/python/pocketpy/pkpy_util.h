#pragma once

#include <pocketpy.h>

#include <seika/assert.h>

#define CRE_PKPY_MODULE_FUNCTION_LIMIT 256

#define PY_ASSERT_NO_EXC() SKA_ASSERT_FMT(!py_checkexc(false), "PKPY Error:\n%s", py_formatexc());

typedef struct CrePPFunction {
    const char* signature;
    py_CFunction function;
} CrePPFunction;

typedef struct CrePPModule {
    const char* name;
    CrePPFunction functions[CRE_PKPY_MODULE_FUNCTION_LIMIT];
} CrePPModule;

void cre_pkpy_util_create_module(CrePPModule* module);
void cre_pkpy_util_create_from_string(const char* moduleName, const char* moduleSource);
