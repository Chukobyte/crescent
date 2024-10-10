#include "pkpy_util.h"

void cre_pkpy_util_create_module(CrePPModule* module) {
    py_Ref newModule = py_newmodule(module->name);
    SKA_ASSERT(newModule);
    for (size_t i = 0; i < CRE_PKPY_MODULE_FUNCTION_LIMIT; i++) {
        const CrePPFunction* func = &module->functions[i];
        if (!func->signature || !func->function) { break; }
        py_bind(newModule, func->signature, func->function);
    }
}

void cre_pkpy_util_create_from_string(const char* moduleName, const char* moduleSource) {
    py_Ref newModule = py_newmodule(moduleName);
    SKA_ASSERT(newModule);
    py_exec(moduleSource, moduleName, EXEC_MODE, newModule);
}
