#include "pkpy_script_context.h"

#include <pocketpy.h>
#include <pocketpy/objects/base.h>

#include <seika/assert.h>
#include <seika/logger.h>
#include <seika/asset/asset_file_loader.h>

#include "pkpy_util.h"
#include "api/pkpy_api.h"

#define PY_ASSERT_NO_EXC() SKA_ASSERT_FMT(!py_checkexc(false), "PKPY Error:\n%s", py_formatexc());

//--- Entity Instance Cache ---//

typedef struct CreEntityInstanceCache {
    py_Ref instances[SKA_MAX_ENTITIES];
} CreEntityInstanceCache;

static CreEntityInstanceCache entityInstanceCache = {0};
static char entityCacheStringBuffer[48];

static void instance_cache_init() {}

static void instance_cache_finalize() {
    memset(entityInstanceCache.instances, 0, sizeof(entityInstanceCache.instances));
}

static py_Ref instance_cache_add(SkaEntity entity,const char* classPath, const char* className) {
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

static void instance_cache_remove(SkaEntity entity) {
    if (entityInstanceCache.instances[entity]) {
        snprintf(entityCacheStringBuffer, sizeof(entityCacheStringBuffer), "_e_%u", entity);
        py_setglobal(py_name(entityCacheStringBuffer), py_None);
        PY_ASSERT_NO_EXC();
        entityInstanceCache.instances[entity] = NULL;
    }
}

static py_Ref instance_cache_get(SkaEntity entity) {
    snprintf(entityCacheStringBuffer, sizeof(entityCacheStringBuffer), "_e_%u", entity);
    py_Ref instance = py_getglobal(py_name(entityCacheStringBuffer));
    PY_ASSERT_NO_EXC();
    return instance;
}

static py_Ref instance_cache_get_checked(SkaEntity entity) {
    SKA_ASSERT(entityInstanceCache.instances[entity]);
    return instance_cache_get(entity);
}

static CREScriptContext* scriptContext = NULL;
static py_Name startFunctionName;
static py_Name processFunctionName;
static py_Name fixedProcessFunctionName;
static py_Name endFunctionName;

static char* pkpy_import_file(const char* path);

//--- Script Context Interface ---//

static void pkpy_init(CREScriptContext* context);
static void pkpy_finalize(CREScriptContext* context);
static void pkpy_create_instance(SkaEntity entity, const char* classPath, const char* className);
static void pkpy_delete_instance(SkaEntity entity);
static void pkpy_on_start(SkaEntity entity);
static void pkpy_on_end(SkaEntity entity);
static void pkpy_on_update(SkaEntity entity, f32 deltaTime);
static void pkpy_on_fixed_update(SkaEntity entity, f32 deltaTime);
static void pkpy_network_callback(const char* message);


CREScriptContextTemplate cre_pkpy_get_script_context_template() {
    return (CREScriptContextTemplate) {
        .contextType = CreScriptContextType_PYTHON,
        .on_script_context_init = pkpy_init,
        .on_script_context_finalize = pkpy_finalize,
        .on_create_instance = pkpy_create_instance,
        .on_delete_instance = pkpy_delete_instance,
        .on_start = pkpy_on_start,
        .on_pre_update_all = NULL,
        .on_post_update_all = NULL,
        .on_update_instance = pkpy_on_update,
        .on_fixed_update_instance = pkpy_on_fixed_update,
        .on_end = pkpy_on_end,
        .on_network_callback = pkpy_network_callback,
    };
}

void pkpy_init(CREScriptContext* context) {
    py_initialize();
    instance_cache_init();
    // Cache function names
    startFunctionName = py_name("_start");
    processFunctionName = py_name("_process");
    fixedProcessFunctionName = py_name("_fixed_process");
    endFunctionName = py_name("_end");
    // Setup callbacks
    py_Callbacks* callbacks = py_callbacks();
    callbacks->importfile = pkpy_import_file;

    // Import internal modules
    cre_pkpy_api_load_internal_modules();

    scriptContext = context;
}

void pkpy_finalize(CREScriptContext* context) {
    scriptContext = NULL;
    instance_cache_finalize();
    py_finalize();
}

void pkpy_create_instance(SkaEntity entity, const char* classPath, const char* className) {
    instance_cache_add(entity, classPath, className);
}

void pkpy_delete_instance(SkaEntity entity) {
    instance_cache_remove(entity);
}

void pkpy_on_start(SkaEntity entity) {
    py_Ref self = instance_cache_get_checked(entity);
    if (py_getattr(self, startFunctionName)) {
        py_push(py_retval());
        py_pushnil();
        py_vectorcall(0, 0);
        PY_ASSERT_NO_EXC();
    } else {
        py_clearexc(NULL);
    }
}

void pkpy_on_end(SkaEntity entity) {
    py_Ref self = instance_cache_get_checked(entity);
    if (py_getattr(self, endFunctionName)) {
        py_push(py_retval());
        py_pushnil();
        py_vectorcall(0, 0);
        PY_ASSERT_NO_EXC();
    } else {
        py_clearexc(NULL);
    }
}

void pkpy_on_update(SkaEntity entity, f32 deltaTime) {
    py_Ref self = instance_cache_get_checked(entity);
    if (py_getattr(self, processFunctionName)) {
        py_Ref pyDeltaTime = NULL;
        py_newfloat(pyDeltaTime, deltaTime);

        py_push(py_retval());
        py_pushnil();
        py_push(pyDeltaTime);
        py_vectorcall(1, 0);
        PY_ASSERT_NO_EXC();
    } else {
        py_clearexc(NULL);
    }
}

void pkpy_on_fixed_update(SkaEntity entity, f32 deltaTime) {
    py_Ref self = instance_cache_get_checked(entity);
    if (py_getattr(self, fixedProcessFunctionName)) {
        py_Ref pyDeltaTime = NULL;
        py_newfloat(pyDeltaTime, deltaTime);

        py_push(py_retval());
        py_pushnil();
        py_push(pyDeltaTime);
        py_vectorcall(1, 0);
        PY_ASSERT_NO_EXC();
    } else {
        py_clearexc(NULL);
    }
}

void pkpy_network_callback(const char* message) {
    SKA_ASSERT_FMT(false, "TODO: Implement!");
}

char* pkpy_import_file(const char* path) {
    // Use built in asset loader to load script instead of pkpy's default
    char* moduleString = ska_asset_file_loader_read_file_contents_as_string(path, NULL);
    SKA_ASSERT_FMT(moduleString, "Failed to load pkpy module at path '%s'", path);
    return moduleString;
}
