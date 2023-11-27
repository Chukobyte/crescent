#include "cre_pkpy_util.h"

#include <string.h>

#include "../seika/utils/se_assert.h"

void cre_copy_name_from_signature(char* target, const char* signature) {
    const char* open_parenthesis = strchr(signature, '(');
    if (open_parenthesis) {
        const size_t nameLength = open_parenthesis - signature;
        strncpy_s(target, sizeof(char) * (nameLength + 1), signature, nameLength);
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
        se_logger_error("python error:\n%s", errorMessage);
        pkpy_free(errorMessage);
        return true;
    }
    return false;
}

void cre_pkpy_util_create_module(pkpy_vm* vm, CrePPModule* module) {
    pkpy_push_module(vm, module->name);
    SE_ASSERT(!cre_pkpy_util_print_error_message(vm));
    pkpy_pop_top(vm);
    char nameBuffer[64];
    strncpy_s(nameBuffer, sizeof(char) * 8, "import ", 8);
    strcat_s(nameBuffer, sizeof(nameBuffer), module->name);
    pkpy_exec(vm, nameBuffer); // import { module->name }
    SE_ASSERT(!cre_pkpy_util_print_error_message(vm));
    for (size_t i = 0; i < CRE_PKPY_MODULE_FUNCTION_LIMIT; i++) {
        const CrePPFunction* func = &module->functions[i];
        if (!func->signature || !func->function) {
            break;
        }
        pkpy_push_function(vm, func->signature, func->function);
        SE_ASSERT(!cre_pkpy_util_print_error_message(vm));
        pkpy_eval(vm, module->name);
        SE_ASSERT(!cre_pkpy_util_print_error_message(vm));

//        SE_ASSERT_FMT(pkpy_stack_size(vm) == 2, "Expected stack size to be 2 but was %d for '%s'", pkpy_stack_size(vm), func->signature);
        cre_copy_name_from_signature(nameBuffer, func->signature);
        pkpy_setattr(vm, pkpy_name(nameBuffer));
        SE_ASSERT(!cre_pkpy_util_print_error_message(vm));
    }
}

void cre_pkpy_util_create_from_string(pkpy_vm* vm, const char* moduleName, const char* moduleSource) {
    const int stackBeforePush = pkpy_stack_size(vm);
    char moduleFileName[64];
    strcpy(moduleFileName, moduleName);
    strcat(moduleFileName, ".py");

    pkpy_push_module(vm, moduleName);
    pkpy_exec_2(vm, moduleSource, moduleFileName, 0, moduleName);
    SE_ASSERT(!cre_pkpy_util_print_error_message(vm));
    pkpy_pop_top(vm);
    SE_ASSERT(stackBeforePush == pkpy_stack_size(vm));
}
