#include "cre_pkpy_util.h"
/*
#include <string.h>

#include <seika/string.h>
#include <seika/logger.h>
#include <seika/assert.h>

void cre_copy_name_from_signature(char* target, const char* signature) {
    const char* open_parenthesis = strchr(signature, '(');
    if (open_parenthesis) {
        const size_t nameLength = open_parenthesis - signature;
        ska_strncpy(target, sizeof(char) * (nameLength + 1), signature, nameLength);
    }
}

char* cre_pkpy_util_get_error_message(pkpy_vm* vm) {
    char* errorMessage = NULL;
    pkpy_clear_error(vm, &errorMessage);
    return errorMessage;
}

bool cre_pkpy_util_print_error_message(pkpy_vm* vm) {
    char* errorMessage = cre_pkpy_util_get_error_message(vm);
    if (errorMessage) {
        ska_logger_error("python error:\n%s", errorMessage);
        pkpy_free(errorMessage);
        return true;
    }
    return false;
}

void cre_pkpy_util_create_module(pkpy_vm* vm, CrePPModule* module) {
    pkpy_push_module(vm, module->name);
    SKA_ASSERT(!cre_pkpy_util_print_error_message(vm));
    pkpy_pop_top(vm);
    char nameBuffer[64];
    ska_strncpy(nameBuffer, sizeof(char) * 8, "import ", 8);
    ska_strcat(nameBuffer, module->name);
    pkpy_exec(vm, nameBuffer); // import { module->name }
    SKA_ASSERT(!cre_pkpy_util_print_error_message(vm));
    for (size_t i = 0; i < CRE_PKPY_MODULE_FUNCTION_LIMIT; i++) {
        const CrePPFunction* func = &module->functions[i];
        if (!func->signature || !func->function) {
            break;
        }
        pkpy_push_function(vm, func->signature, func->function);
        SKA_ASSERT(!cre_pkpy_util_print_error_message(vm));
        pkpy_eval(vm, module->name);
        SKA_ASSERT(!cre_pkpy_util_print_error_message(vm));

        cre_copy_name_from_signature(nameBuffer, func->signature);
        pkpy_setattr(vm, pkpy_name(nameBuffer));
        SKA_ASSERT(!cre_pkpy_util_print_error_message(vm));
    }
}

void cre_pkpy_util_create_from_string(pkpy_vm* vm, const char* moduleName, const char* moduleSource) {
    const int32 stackBeforePush = pkpy_stack_size(vm);
    char moduleFileName[64];
    ska_strcpy(moduleFileName, moduleName);
    ska_strcat(moduleFileName, ".py");

    pkpy_push_module(vm, moduleName);
    pkpy_exec_2(vm, moduleSource, moduleFileName, 0, moduleName);
    SKA_ASSERT(!cre_pkpy_util_print_error_message(vm));
    pkpy_pop_top(vm);
    SKA_ASSERT(stackBeforePush == pkpy_stack_size(vm));
}
*/
