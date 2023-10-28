#include "cre_py_pp_util.h"

#include "../seika/src/utils/se_assert.h"

char* cre_py_pp_util_get_error_message(pkpy_vm* vm) {
    char* errorMessage = NULL;
    pkpy_clear_error(vm, &errorMessage);
    return errorMessage;
}

bool cre_py_pp_util_print_error_message(pkpy_vm* vm) {
    char* errorMessage = cre_py_pp_util_get_error_message(vm);
    if (errorMessage) {
        se_logger_error("python error:\n%s", errorMessage);
        pkpy_free(errorMessage);
        return true;
    }
    return false;
}

void cre_py_pp_util_create_module(pkpy_vm* vm, CrePPModule* module) {
    pkpy_push_module(vm, module->name);
    SE_ASSERT(!cre_py_pp_util_print_error_message(vm));
    pkpy_pop_top(vm);
    pkpy_exec(vm, "import crescent_api_internal");
    SE_ASSERT(!cre_py_pp_util_print_error_message(vm));
    for (size_t i = 0; i < module->functionCount; i++) {
        const CrePPFunction* func = &module->functions[i];
        pkpy_push_function(vm, func->signature, func->function);
        SE_ASSERT(!cre_py_pp_util_print_error_message(vm));
        pkpy_eval(vm, module->name);
        SE_ASSERT(!cre_py_pp_util_print_error_message(vm));

        SE_ASSERT_FMT(pkpy_stack_size(vm) == 2, "Expected stack size to be 2 but was %d for '%s'", pkpy_stack_size(vm), func->name);
        pkpy_setattr(vm, pkpy_name(func->name));
        SE_ASSERT(!cre_py_pp_util_print_error_message(vm));
    }
    // Testing adding a module
//    pkpy_push_function(vm, "node_get_name(entity_id: int) -> str", pocketpy_test_node_get_name);
//    TEST_ASSERT_FALSE(print_py_error_message(vm));
//    pkpy_push_module(vm, "crescent_api_internal");
//    TEST_ASSERT_FALSE(print_py_error_message(vm));
//    pkpy_setattr(vm, pkpy_name("node_get_name"));
//    TEST_ASSERT_FALSE(print_py_error_message(vm));
//
//    pkpy_push_function(vm, "node_get_children(entity_id: int) -> Tuple[int, ...]", pocketpy_test_node_get_children);
//    TEST_ASSERT_FALSE(print_py_error_message(vm));
//    pkpy_exec(vm, "import crescent_api_internal");
//    TEST_ASSERT_FALSE(print_py_error_message(vm));
//    pkpy_eval(vm, "crescent_api_internal");
//    TEST_ASSERT_FALSE(print_py_error_message(vm));
//    TEST_ASSERT_EQUAL_INT(2, pkpy_stack_size(vm));
//    pkpy_setattr(vm, pkpy_name("node_get_children"));
//    TEST_ASSERT_FALSE(print_py_error_message(vm));
//
//    TEST_ASSERT_EQUAL_INT(0, pkpy_stack_size(vm));
}
