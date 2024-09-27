#pragma once

#include <pocketpy.h>

#include <seika/string.h>
#include <seika/assert.h>

#define CRE_PKPY_MODULE_FUNCTION_LIMIT 256

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
