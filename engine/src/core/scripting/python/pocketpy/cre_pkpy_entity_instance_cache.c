#include "cre_pkpy_entity_instance_cache.h"

#include <stdio.h>

#include <seika/ecs/ecs.h>
#include <seika/utils/se_assert.h>

#include "cre_pkpy.h"
#include "cre_pkpy_util.h"
#include "cre_pkpy_api_source.h"
#include "../../../ecs/ecs_globals.h"
//#include "../../../ecs/system/ec_system.h"
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
    SE_ASSERT(entity != CRE_NULL_ENTITY);
    // import module first
    char importCmdBuffer[96];
    sprintf(importCmdBuffer, "from %s import %s", classPath, className);
    pkpy_exec(vm, importCmdBuffer);
    SE_ASSERT(!cre_pkpy_util_print_error_message(vm));

    pkpy_getglobal(vm, pkpy_name(className));
    pkpy_push_null(vm);
    pkpy_push_int(vm, (int)entity);
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

    // Add script component if missing
    if (!ska_ecs_component_manager_has_component(entity, SCRIPT_COMPONENT_INDEX)) {
        ska_ecs_component_manager_set_component(entity, SCRIPT_COMPONENT_INDEX, script_component_create_ex(classPath, className, ScriptContextType_PYTHON));
    }

    return entity;
}

bool cre_pkpy_entity_instance_cache_create_new_entity_if_nonexistent(pkpy_vm* vm, const char* classPath, const char* className, CreEntity entity) {
    if (!cre_pkpy_entity_instance_cache_has_entity(vm, entity)) {
        cre_pkpy_entity_instance_cache_create_new_entity(vm, classPath, className, entity);
        return true;
    }
    return false;
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
