#include "entity_instance_cache.h"

#include <stdio.h>

#include "../seika/src/utils/se_assert.h"

#include "cre_py_pp_util.h"

static bool entity_instance_cache_is_initialized = false;

void cre_pypp_entity_instance_cache_initialize(pkpy_vm* vm) {
    if (!entity_instance_cache_is_initialized) {
        pkpy_exec(vm, "from typing import Dict");
        SE_ASSERT(!cre_py_pp_util_print_error_message(vm));
        pkpy_exec(vm, "from crescent import Node");
        SE_ASSERT(!cre_py_pp_util_print_error_message(vm));
        pkpy_exec(vm, "CRE_ENTITY_TO_NODE_MAP: Dict[int, Node]");
        SE_ASSERT(!cre_py_pp_util_print_error_message(vm));

#define CRE_PY_PP_GET_ENTITY_FUNC "" \
"def cre_global_get_entity(entity_id: int) -> Node:"\
    "return CRE_ENTITY_TO_NODE_MAP[entity_id]" \
""
        pkpy_exec(vm, CRE_PY_PP_GET_ENTITY_FUNC);
        SE_ASSERT(!cre_py_pp_util_print_error_message(vm));
#undef CRE_PY_PP_GET_ENTITY_FUNC

        entity_instance_cache_is_initialized = true;
    }
}

void cre_pypp_entity_instance_cache_finalize(pkpy_vm* vm) {
    if (entity_instance_cache_is_initialized) {
        pkpy_exec(vm, "CRE_ENTITY_TO_NODE_MAP.clear()");
        SE_ASSERT(!cre_py_pp_util_print_error_message(vm));
        entity_instance_cache_is_initialized = false;
    }
}

void cre_pypp_entity_instance_cache_add_entity(pkpy_vm* vm, CreEntity entity) {

}

void cre_pypp_entity_instance_cache_remove_entity(pkpy_vm* vm, CreEntity entity) {

}

void cre_pypp_entity_instance_cache_push_entity_instance(pkpy_vm* vm, CreEntity entity) {
    char entityStringBuffer[32];
    sprintf(entityStringBuffer, "CRE_ENTITY_TO_NODE_MAP[%d]", entity);
    pkpy_eval(vm, entityStringBuffer);
    SE_ASSERT(!cre_py_pp_util_print_error_message(vm));
}
