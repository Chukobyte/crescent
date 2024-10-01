#include "pkpy_instance_cache.h"

#include <seika/logger.h>

#include "pkpy_util.h"

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
    if (entityInstanceCache.instances[entity] != NULL) {
        ska_logger_warn("Attempting to add entity '%u' from '%s.%s'!", entity, classPath, className);
        return entityInstanceCache.instances[entity];
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
    SKA_ASSERT(entityInstanceCache.instances[entity]);
    return cre_pkpy_instance_cache_get(entity);
}
