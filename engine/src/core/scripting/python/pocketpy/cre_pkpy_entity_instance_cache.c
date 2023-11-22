#include "cre_pkpy_entity_instance_cache.h"

#include <stdio.h>

#include "../seika/utils/se_assert.h"

#include "cre_pkpy.h"
#include "cre_pkpy_util.h"
#include "cre_pkpy_api_source.h"
#include "../../../ecs/system/ec_system.h"
#include "../../../ecs/component/node_component.h"
#include "../../../ecs/component/script_component.h"

static bool entity_instance_cache_is_initialized = false;

void cre_pkpy_entity_instance_cache_initialize(pkpy_vm* vm) {
    if (!entity_instance_cache_is_initialized) {
        pkpy_push_module(vm, "crescent_internal_py");
        pkpy_exec_2(vm, CRE_PKPY_CRESCENT_INTERNAL_PY_SOURCE, "crescent_internal_py.py", 0, "crescent_internal_py");
        SE_ASSERT(!cre_pkpy_util_print_error_message(vm));
        pkpy_exec(vm, "import crescent_internal_py");
        SE_ASSERT(!cre_pkpy_util_print_error_message(vm));
        pkpy_pop_top(vm);
        entity_instance_cache_is_initialized = true;
    }
}

void cre_pkpy_entity_instance_cache_finalize(pkpy_vm* vm) {
    if (entity_instance_cache_is_initialized) {
        pkpy_exec(vm, "crescent_internal_py.CRE_ENTITY_TO_NODE_MAP.clear()");
        SE_ASSERT(!cre_pkpy_util_print_error_message(vm));
        entity_instance_cache_is_initialized = false;
    }
}

CreEntity cre_pkpy_entity_instance_cache_create_new_entity(pkpy_vm* vm, const char* classPath, const char* className, CreEntity entity) {
    // import module first
    char importCmdBuffer[96];
    sprintf(importCmdBuffer, "from %s import %s", classPath, className);
    pkpy_exec(vm, importCmdBuffer);
    SE_ASSERT(!cre_pkpy_util_print_error_message(vm));

    const CreEntity newEntity = entity != CRE_NULL_ENTITY ? entity : cre_ec_system_create_entity_uid();
    pkpy_getglobal(vm, pkpy_name(className));
    pkpy_push_null(vm);
    pkpy_push_int(vm, (int)newEntity);
    pkpy_vectorcall(vm, 1);
    SE_ASSERT(!cre_pkpy_util_print_error_message(vm));

    pkpy_eval(vm, "crescent_internal_py");
    SE_ASSERT(!cre_pkpy_util_print_error_message(vm));
    pkpy_getattr(vm, pkpy_name("set_entity"));
    SE_ASSERT(!cre_pkpy_util_print_error_message(vm));
    pkpy_rot_two(vm);
    SE_ASSERT(!cre_pkpy_util_print_error_message(vm));
    pkpy_push_null(vm);
    SE_ASSERT(!cre_pkpy_util_print_error_message(vm));
    pkpy_rot_two(vm);
    SE_ASSERT(!cre_pkpy_util_print_error_message(vm));
    pkpy_vectorcall(vm, 1);
    SE_ASSERT(!cre_pkpy_util_print_error_message(vm));
    pkpy_pop_top(vm);
    return newEntity;
}

void cre_pkpy_entity_instance_cache_remove_entity(pkpy_vm* vm, CreEntity entity) {
    char entityStringBuffer[56];
    sprintf(entityStringBuffer, "del crescent_internal_py.CRE_ENTITY_TO_NODE_MAP[%d]", entity);
    pkpy_exec(vm, entityStringBuffer);
    SE_ASSERT(!cre_pkpy_util_print_error_message(vm));
}

bool cre_pkpy_entity_instance_cache_has_entity(pkpy_vm *vm, CreEntity entity) {
    const int stackSizeBeforeEval = pkpy_stack_size(vm);
    char entityStringBuffer[56];
    sprintf(entityStringBuffer, "%d in crescent_internal_py.CRE_ENTITY_TO_NODE_MAP", entity);
    pkpy_eval(vm, entityStringBuffer);
    SE_ASSERT(!cre_pkpy_util_print_error_message(vm));
    bool doesKeyExist = false;
    pkpy_to_bool(vm, stackSizeBeforeEval, &doesKeyExist);
    SE_ASSERT(!cre_pkpy_util_print_error_message(vm));
    pkpy_pop_top(vm);
    return doesKeyExist;
}

void cre_pkpy_entity_instance_cache_push_entity_instance(pkpy_vm* vm, CreEntity entity) {
    char entityStringBuffer[56];
    sprintf(entityStringBuffer, "crescent_internal_py.CRE_ENTITY_TO_NODE_MAP[%d]", entity);
    pkpy_eval(vm, entityStringBuffer);
    SE_ASSERT(!cre_pkpy_util_print_error_message(vm));
}

void cre_pkpy_entity_instance_cache_push_or_add_default_entity_instance(pkpy_vm* vm, CreEntity entity) {
    if (!cre_pkpy_entity_instance_cache_has_entity(vm, entity)) {
        const NodeComponent* nodeComponent = cre_component_manager_get_component_unchecked(entity, CreComponentDataIndex_NODE);
        SE_ASSERT(nodeComponent);
        const char* nodeClassPath = CRE_PKPY_MODULE_NAME_CRESCENT;
        const char* nodeBaseType = node_get_base_type_string(nodeComponent->type);
        cre_component_manager_set_component(entity, CreComponentDataIndex_SCRIPT, script_component_create_ex(nodeClassPath, nodeBaseType, ScriptContextType_PYTHON));
        cre_pkpy_entity_instance_cache_create_new_entity(vm, nodeClassPath, nodeBaseType, entity);
    }
    cre_pkpy_entity_instance_cache_push_entity_instance(vm, entity);
}
