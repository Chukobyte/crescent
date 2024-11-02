#include "pkpy_instance_cache.h"

#include <stdio.h>

#include <seika/logger.h>
#include <seika/ecs/component.h>

#include "cre_pkpy.h"
#include "pkpy_util.h"
#include "core/scripting/script_context.h"
#include "core/ecs/ecs_globals.h"
#include "core/ecs/components/node_component.h"
#include "core/ecs/components/script_component.h"

typedef struct CreEntityInstanceCache {
    py_Ref instances[SKA_MAX_ENTITIES];
} CreEntityInstanceCache;

static CreEntityInstanceCache entityInstanceCache = {0};
static char entityCacheStringBuffer[48];

void cre_pkpy_instance_cache_init() {}

void cre_pkpy_instance_cache_finalize() {
    memset(entityInstanceCache.instances, 0, sizeof(entityInstanceCache.instances));
}

py_Ref cre_pkpy_instance_cache_add(SkaEntity entity,const char* classPath, const char* className) {
    // Early out if entity already exists
    if (cre_pkpy_instance_cache_has(entity)) {
        ska_logger_info("Attempted to add entity '%u' from '%s.%s', but it already exists!", entity, classPath, className);
        return cre_pkpy_instance_cache_get(entity);
    }

    snprintf(entityCacheStringBuffer, sizeof(entityCacheStringBuffer), "from %s import %s", classPath, className);
    py_exec(entityCacheStringBuffer, "main.py", EXEC_MODE, NULL);
    PY_ASSERT_NO_EXC();
    snprintf(entityCacheStringBuffer, sizeof(entityCacheStringBuffer), "_e_%u = %s(%u)", entity, className, entity);
    py_exec(entityCacheStringBuffer, "main.py", EXEC_MODE, NULL);
    PY_ASSERT_NO_EXC();

    snprintf(entityCacheStringBuffer, sizeof(entityCacheStringBuffer), "_e_%u", entity);
    py_Ref instanceRef = py_getglobal(py_name(entityCacheStringBuffer));
    PY_ASSERT_NO_EXC();

    SKA_ASSERT_FMT(instanceRef, "Unable to create instance from '%s.%s'!", classPath, className);
    entityInstanceCache.instances[entity] = instanceRef;

    return instanceRef;
}

py_Ref cre_pkpy_instance_cache_add2(SkaEntity entity) {
    // Early out if entity already exists
    if (cre_pkpy_instance_cache_has(entity)) {
        const ScriptComponent* scriptComponent = (ScriptComponent*)ska_ecs_component_manager_get_component(entity, SCRIPT_COMPONENT_INDEX);
        ska_logger_info("Attempted to add entity '%u' from '%s.%s', but it already exists!", entity, scriptComponent->classPath, scriptComponent->className);
        return cre_pkpy_instance_cache_get(entity);
    }

    const NodeComponent* nodeComponent = (NodeComponent*)ska_ecs_component_manager_get_component(entity, NODE_COMPONENT_INDEX);
    ScriptComponent* scriptComponent = (ScriptComponent*)ska_ecs_component_manager_get_component_unchecked(entity, SCRIPT_COMPONENT_INDEX);
    if (!scriptComponent) {
        // If an entity doesn't have a script component at this point, it should just be a plain base class from the crescent module.
        // Since that is the case, just create a script component so that we can clean up within the script context when the entity
        // needs to leave the instance cache.
        const char* baseClassName = node_get_base_type_string(nodeComponent->type);
        scriptComponent = script_component_create_ex(CRE_PKPY_MODULE_NAME_CRESCENT, baseClassName, CreScriptContextType_PYTHON);
        ska_ecs_component_manager_set_component(entity, SCRIPT_COMPONENT_INDEX, scriptComponent);
    }
    return cre_pkpy_instance_cache_add(entity, scriptComponent->classPath, scriptComponent->className);
}

void cre_pkpy_instance_cache_remove(SkaEntity entity) {
    if (entityInstanceCache.instances[entity]) {
        snprintf(entityCacheStringBuffer, sizeof(entityCacheStringBuffer), "_e_%u", entity);
        py_setglobal(py_name(entityCacheStringBuffer), py_None);
        PY_ASSERT_NO_EXC();
        entityInstanceCache.instances[entity] = NULL;
    }
}

py_Ref cre_pkpy_instance_cache_get(SkaEntity entity) {
    snprintf(entityCacheStringBuffer, sizeof(entityCacheStringBuffer), "_e_%u", entity);
    py_Ref instance = py_getglobal(py_name(entityCacheStringBuffer));
    PY_ASSERT_NO_EXC();
    return instance;
}

py_Ref cre_pkpy_instance_cache_get_checked(SkaEntity entity) {
    SKA_ASSERT(cre_pkpy_instance_cache_has(entity));
    return cre_pkpy_instance_cache_get(entity);
}

bool cre_pkpy_instance_cache_has(SkaEntity entity) {
    return entityInstanceCache.instances[entity] != NULL;
}
