#include "entity_instance_cache.h"

#include <stdio.h>

#include "../seika/src/utils/se_assert.h"

#include "cre_py_pp_util.h"
#include "../../../ecs/system/ec_system.h"
#include "crescent_api_source.h"

static bool entity_instance_cache_is_initialized = false;

void cre_pypp_entity_instance_cache_initialize(pkpy_vm* vm) {
    if (!entity_instance_cache_is_initialized) {
        SE_ASSERT(pkpy_stack_size(vm) == 0);
        pkpy_push_module(vm, "crescent_internal_py");
        pkpy_exec_2(vm, CRE_PYPP_CRESCENT_INTERNAL_PY_SOURCE, "crescent_internal_py.py", 0, "crescent_internal_py");
        SE_ASSERT(!cre_py_pp_util_print_error_message(vm));
        pkpy_exec(vm, "import crescent_internal_py");
        SE_ASSERT(!cre_py_pp_util_print_error_message(vm));
        pkpy_pop_top(vm);
        SE_ASSERT(pkpy_stack_size(vm) == 0);
        entity_instance_cache_is_initialized = true;
    }
}

void cre_pypp_entity_instance_cache_finalize(pkpy_vm* vm) {
    if (entity_instance_cache_is_initialized) {
        pkpy_exec(vm, "crescent_internal_py.CRE_ENTITY_TO_NODE_MAP.clear()");
        SE_ASSERT(!cre_py_pp_util_print_error_message(vm));
        entity_instance_cache_is_initialized = false;
    }
}

CreEntity cre_pypp_entity_instance_cache_create_new_entity(pkpy_vm* vm, const char* className) {
    const CreEntity newEntity = cre_ec_system_create_entity_uid();
    pkpy_getglobal(vm, pkpy_name(className));
    pkpy_push_null(vm);
    pkpy_push_int(vm, (int)newEntity);
    pkpy_vectorcall(vm, 1);
    SE_ASSERT(!cre_py_pp_util_print_error_message(vm));
    SE_ASSERT(pkpy_stack_size(vm) == 1);

    pkpy_eval(vm, "crescent_internal_py");
    SE_ASSERT(!cre_py_pp_util_print_error_message(vm));
    pkpy_getattr(vm, pkpy_name("set_entity"));
    SE_ASSERT(!cre_py_pp_util_print_error_message(vm));
    SE_ASSERT(pkpy_stack_size(vm) == 2);
    pkpy_rot_two(vm);
    SE_ASSERT(!cre_py_pp_util_print_error_message(vm));
    pkpy_push_null(vm);
    SE_ASSERT(!cre_py_pp_util_print_error_message(vm));
    pkpy_rot_two(vm);
    SE_ASSERT(!cre_py_pp_util_print_error_message(vm));
    pkpy_vectorcall(vm, 1);
    SE_ASSERT(!cre_py_pp_util_print_error_message(vm));
    pkpy_pop_top(vm);
    SE_ASSERT(pkpy_stack_size(vm) == 0);
    return newEntity;
}

void cre_pypp_entity_instance_cache_remove_entity(pkpy_vm* vm, CreEntity entity) {
    char entityStringBuffer[56];
    sprintf(entityStringBuffer, "del crescent_internal_py.CRE_ENTITY_TO_NODE_MAP[%d]", entity);
    pkpy_exec(vm, entityStringBuffer);
    SE_ASSERT(!cre_py_pp_util_print_error_message(vm));
}

void cre_pypp_entity_instance_cache_push_entity_instance(pkpy_vm* vm, CreEntity entity) {
    char entityStringBuffer[56];
    sprintf(entityStringBuffer, "crescent_internal_py.CRE_ENTITY_TO_NODE_MAP[%d]", entity);
    pkpy_eval(vm, entityStringBuffer);
    SE_ASSERT(!cre_py_pp_util_print_error_message(vm));
}
