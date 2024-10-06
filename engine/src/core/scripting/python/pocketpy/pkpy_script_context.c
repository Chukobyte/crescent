#include "pkpy_script_context.h"

#include <pocketpy.h>

#include <seika/logger.h>
#include <seika/asset/asset_file_loader.h>

#include "pkpy_util.h"
#include "pkpy_instance_cache.h"
#include "../../../../../../cmake-build-release-msvc/_deps/seika_content-src/seika/data_structures/array_utils.h"
#include "api/pkpy_api.h"

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
    cre_pkpy_instance_cache_init();
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
    cre_pkpy_instance_cache_finalize();
    py_finalize();
}

void pkpy_create_instance(SkaEntity entity, const char* classPath, const char* className) {
    py_Ref newInstance = cre_pkpy_instance_cache_add(entity, classPath, className);
    if (py_getattr(newInstance, processFunctionName)) {
        scriptContext->updateEntities[scriptContext->updateEntityCount++] = entity;
    } else {
        py_clearexc(NULL);
    }
    if (py_getattr(newInstance, fixedProcessFunctionName)) {
        scriptContext->fixedUpdateEntities[scriptContext->fixedUpdateEntityCount++] = entity;
    } else {
        py_clearexc(NULL);
    }
}

void pkpy_delete_instance(SkaEntity entity) {
    cre_pkpy_instance_cache_remove(entity);
    ska_array_utils_remove_item_uint32(
        scriptContext->updateEntities,
        &scriptContext->updateEntityCount,
        entity,
        SKA_NULL_ENTITY
    );
    ska_array_utils_remove_item_uint32(
        scriptContext->fixedUpdateEntities,
        &scriptContext->fixedUpdateEntityCount,
        entity,
        SKA_NULL_ENTITY
    );
}

void pkpy_on_start(SkaEntity entity) {
    py_Ref self = cre_pkpy_instance_cache_get_checked(entity);
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
    py_Ref self = cre_pkpy_instance_cache_get_checked(entity);
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
    py_Ref self = cre_pkpy_instance_cache_get_checked(entity);
    if (py_getattr(self, processFunctionName)) {
        py_Ref pyDeltaTime = py_pushtmp();
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
    py_Ref self = cre_pkpy_instance_cache_get_checked(entity);
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
