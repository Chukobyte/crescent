#include "py_cache.h"

#include <Python.h>

#include "../seika/src/data_structures/se_hash_map_string.h"
#include "../seika/src/memory/se_mem.h"
#include "../seika/src/utils/se_assert.h"

typedef struct PyModuleCacheItem {
    PyObject* module;
    SEStringHashMap* classHashMap;
} PyModuleCacheItem;

static SEStringHashMap* pyModuleCacheHashMap = NULL;

void rbe_py_cache_initialize() {
    pyModuleCacheHashMap = se_string_hash_map_create(128);
}

void rbe_py_cache_finalize() {
    se_string_hash_map_destroy(pyModuleCacheHashMap);
}

PyObject* rbe_py_cache_get_module(const char* modulePath) {
    if (!se_string_hash_map_has(pyModuleCacheHashMap, modulePath)) {
        PyObject* pName = PyUnicode_FromString(modulePath);
        PyObject* pNewModule = PyImport_Import(pName);
        // Fail over import
        if (pNewModule == NULL) {
            se_logger_debug("Using fail over import for module at path '%s'", modulePath);
            PyObject* fromListObject = PyUnicode_FromString("[*]");
            pNewModule = PyImport_ImportModuleEx(modulePath, NULL, NULL, fromListObject);
            PyErr_Print();
        }
        SE_ASSERT_FMT(pNewModule != NULL, "Failed to import python module at path '%s'", modulePath);

        const size_t cacheItemSize = sizeof(PyModuleCacheItem*) + sizeof(PyObject*);
        PyModuleCacheItem* cacheItem = SE_MEM_ALLOCATE_SIZE(cacheItemSize);
        cacheItem->module = pNewModule;
        cacheItem->classHashMap = se_string_hash_map_create(16);

        se_string_hash_map_add(pyModuleCacheHashMap, modulePath, cacheItem, cacheItemSize);
        SE_MEM_FREE(cacheItem);
        // Increase ref to store in cache and to return to caller
        Py_IncRef(pNewModule);
        Py_DecRef(pName);
    }
    PyModuleCacheItem* moduleCacheItem = (PyModuleCacheItem*) se_string_hash_map_get(pyModuleCacheHashMap, modulePath);
    SE_ASSERT(moduleCacheItem != NULL);
    SE_ASSERT_FMT(moduleCacheItem->module != NULL, "Unable to get module at path '%s'", modulePath);
    return moduleCacheItem->module;
}

PyObject* rbe_py_cache_get_class(const char* modulePath, const char* classPath) {
    PyObject* pyModule = rbe_py_cache_get_module(modulePath);
    PyModuleCacheItem* moduleCacheItem = (PyModuleCacheItem*) se_string_hash_map_get(pyModuleCacheHashMap, modulePath);
    if (!se_string_hash_map_has(moduleCacheItem->classHashMap, classPath)) {
        PyObject* pModuleDict = PyModule_GetDict(pyModule);
        SE_ASSERT(pModuleDict != NULL);
        PyObject* pNewClass = PyDict_GetItemString(pModuleDict, classPath);
        SE_ASSERT(pNewClass != NULL);
        // Store a pointer to the PyObject pointer to not worry about PyObject size...
        se_string_hash_map_add(moduleCacheItem->classHashMap, classPath, &pNewClass, sizeof(PyObject **));
//        Py_DecRef(pModuleDict); // TODO: This crashed on linux, find out what the deal is.
    }
    PyObject** pClass = (PyObject**) se_string_hash_map_get(moduleCacheItem->classHashMap, classPath);
    SE_ASSERT(pClass != NULL);
    return *pClass;
}

PyObject* rbe_py_cache_create_instance(const char* modulePath, const char* classPath, Entity entity) {
    PyObject* argsList = Py_BuildValue("(i)", entity);
    SE_ASSERT(argsList != NULL);
    PyObject* classRef = rbe_py_cache_get_class(modulePath, classPath);
    PyObject* classInstance = PyObject_CallObject(classRef, argsList);
    SE_ASSERT(classInstance != NULL);
    SE_ASSERT(PyObject_IsInstance(classInstance, classRef));
    Py_IncRef(classInstance);
    return classInstance;
}
