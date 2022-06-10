#include "py_cache.h"

#include <Python.h>

#include "../../data_structures/rbe_hash_map_string.h"
#include "../../memory/rbe_mem.h"
#include "../../utils/rbe_assert.h"

typedef struct PyModuleCacheItem {
    PyObject** module;
    RBEStringHashMap* classHashMap;
} PyModuleCacheItem;

static RBEStringHashMap* pyModuleCacheHashMap = NULL;

void rbe_py_cache_initialize() {
    pyModuleCacheHashMap = rbe_string_hash_map_create(128);
}

void rbe_py_cache_finalize() {
    rbe_string_hash_map_destroy(pyModuleCacheHashMap);
}

PyObject* rbe_py_cache_get_module(const char* modulePath) {
    if (!rbe_string_hash_map_has(pyModuleCacheHashMap, modulePath)) {
        PyObject* pName = PyUnicode_FromString(modulePath);
        PyObject* pNewModule = PyImport_Import(pName);
        RBE_ASSERT(pNewModule != NULL);

        size_t cacheItemSize = sizeof(PyModuleCacheItem*) + sizeof(PyObject**);
        PyModuleCacheItem* cacheItem = RBE_MEM_ALLOCATE_SIZE(cacheItemSize);
        cacheItem->module = &pNewModule;
        cacheItem->classHashMap = rbe_string_hash_map_create(16);

        rbe_string_hash_map_add(pyModuleCacheHashMap, modulePath, cacheItem, cacheItemSize);
        // Increase ref to store in cache and to return to caller
        Py_IncRef(pNewModule);
        Py_DecRef(pName);
    }
    PyModuleCacheItem* moduleCacheItem = (PyModuleCacheItem*) rbe_string_hash_map_get(pyModuleCacheHashMap, modulePath);
    RBE_ASSERT(moduleCacheItem != NULL);
    RBE_ASSERT(moduleCacheItem->module != NULL);
    return *moduleCacheItem->module;
}

PyObject* rbe_py_cache_get_class(const char* modulePath, const char* classPath) {
    RBE_ASSERT_FMT(rbe_py_cache_get_module(modulePath) != NULL, "Unable to load module cache item '%s' for class '%s'", modulePath, classPath);
    PyModuleCacheItem* moduleCacheItem = (PyModuleCacheItem*) rbe_string_hash_map_get(pyModuleCacheHashMap, modulePath);
    if (!rbe_string_hash_map_has(moduleCacheItem->classHashMap, classPath)) {
        PyObject* pModuleDict = PyModule_GetDict(*moduleCacheItem->module);
        RBE_ASSERT(pModuleDict != NULL);
        PyObject* pNewClass = PyDict_GetItemString(pModuleDict, classPath);
        RBE_ASSERT(pNewClass != NULL);
        // Store a pointer to the PyObject pointer to not worry about PyObject size...
        rbe_string_hash_map_add(moduleCacheItem->classHashMap, classPath, &pNewClass, sizeof(PyObject**));
        Py_DecRef(pModuleDict);
    }
    PyObject** pClass = (PyObject**) rbe_string_hash_map_get(moduleCacheItem->classHashMap, classPath);
    RBE_ASSERT(pClass != NULL);
    return *pClass;
}

PyObject* rbe_py_cache_create_instance(const char* modulePath, const char* classPath, Entity entity) {
    PyObject* argsList = Py_BuildValue("(i)", entity);
    RBE_ASSERT(argsList != NULL);
    PyObject* classRef = rbe_py_cache_get_class(modulePath, classPath);
    PyObject* classInstance = PyObject_CallObject(classRef, argsList);
    RBE_ASSERT(classInstance != NULL);
    RBE_ASSERT(PyObject_IsInstance(classInstance, classRef));
    Py_IncRef(classInstance);
    return classInstance;
}
