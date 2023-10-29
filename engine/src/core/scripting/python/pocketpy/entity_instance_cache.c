#include "entity_instance_cache.h"

#include <stdio.h>

#include "../seika/src/utils/se_assert.h"

#include "cre_py_pp_util.h"
#include "../../../ecs/system/ec_system.h"

static bool entity_instance_cache_is_initialized = false;

void cre_pypp_entity_instance_cache_initialize(pkpy_vm* vm) {
    if (!entity_instance_cache_is_initialized) {
        pkpy_exec(vm, "from typing import Dict");
        SE_ASSERT(!cre_py_pp_util_print_error_message(vm));
        pkpy_exec(vm, "from crescent import Node");
        SE_ASSERT(!cre_py_pp_util_print_error_message(vm));
        pkpy_exec(vm, "CRE_ENTITY_TO_NODE_MAP: Dict[int, Node]");
        SE_ASSERT(!cre_py_pp_util_print_error_message(vm));

#define CRE_PY_PP_GLOBAL_FUNCS "" \
"def cre_global_get_entity(entity_id: int) -> Node:"\
    "return CRE_ENTITY_TO_NODE_MAP[entity_id]" \
""\
  "def cre_global_set_entity(node: Node) -> None:"\
    "CRE_ENTITY_TO_NODE_MAP[node.entity_id] = node" \
""
        pkpy_exec(vm, CRE_PY_PP_GLOBAL_FUNCS);
        SE_ASSERT(!cre_py_pp_util_print_error_message(vm));
#undef CRE_PY_PP_GLOBAL_FUNCS

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

CreEntity cre_pypp_entity_instance_cache_create_new_entity(pkpy_vm* vm, const char* className) {
    const CreEntity newEntity = cre_ec_system_create_entity_uid();
    pkpy_getglobal(vm, pkpy_name(className));
    pkpy_push_null(vm);
    pkpy_push_int(vm, (int)newEntity);
    pkpy_vectorcall(vm, 1);
    SE_ASSERT(!cre_py_pp_util_print_error_message(vm));

    pkpy_getglobal(vm, pkpy_name("cre_global_set_entity"));
    pkpy_rot_two(vm);
    pkpy_vectorcall(vm, 1);
    SE_ASSERT(!cre_py_pp_util_print_error_message(vm));
    return newEntity;
}

void cre_pypp_entity_instance_cache_remove_entity(pkpy_vm* vm, CreEntity entity) {
    char entityStringBuffer[32];
    sprintf(entityStringBuffer, "del CRE_ENTITY_TO_NODE_MAP[%d]", entity);
    pkpy_exec(vm, entityStringBuffer);
    SE_ASSERT(!cre_py_pp_util_print_error_message(vm));
}

void cre_pypp_entity_instance_cache_push_entity_instance(pkpy_vm* vm, CreEntity entity) {
    char entityStringBuffer[32];
    sprintf(entityStringBuffer, "CRE_ENTITY_TO_NODE_MAP[%d]", entity);
    pkpy_eval(vm, entityStringBuffer);
    SE_ASSERT(!cre_py_pp_util_print_error_message(vm));
}
