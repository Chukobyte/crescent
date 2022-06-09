#include "py_cache.h"

#include <Python.h>

#include "../../data_structures/rbe_hash_map_string.h"
#include "../../memory/rbe_mem.h"
#include "../../utils/rbe_assert.h"

typedef struct PyModuleCacheItem {
    PyObject* module;
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

        PyModuleCacheItem* cacheItem = RBE_MEM_ALLOCATE_SIZE(sizeof(PyModuleCacheItem*) + sizeof(*pNewModule));
        cacheItem->module = pNewModule;
        cacheItem->classHashMap = rbe_string_hash_map_create(16);

        rbe_string_hash_map_add(pyModuleCacheHashMap, modulePath, cacheItem, sizeof(*cacheItem));
        Py_DecRef(pName);
        // Increase ref to store in cache and to return to caller
        Py_IncRef(pNewModule);
    }
    PyModuleCacheItem* moduleCacheItem = (PyModuleCacheItem*) rbe_string_hash_map_get(pyModuleCacheHashMap, modulePath);
    RBE_ASSERT(moduleCacheItem != NULL);
    RBE_ASSERT(moduleCacheItem->module != NULL);
    return moduleCacheItem->module;
}

PyObject* rbe_py_cache_get_module_old(const char* modulePath) {
    if (!rbe_string_hash_map_has(pyModuleCacheHashMap, modulePath)) {
        PyObject* pName = PyUnicode_FromString(modulePath);
        PyObject* pNewModule = PyImport_Import(pName);
        RBE_ASSERT(pNewModule != NULL);
        rbe_string_hash_map_add(pyModuleCacheHashMap, modulePath, pNewModule, sizeof(*pNewModule));
        Py_DecRef(pName);
        // Increase ref to store in cache and to return to caller
        Py_IncRef(pNewModule);
    }
    PyObject* pModule = (PyObject*) rbe_string_hash_map_get(pyModuleCacheHashMap, modulePath);
    RBE_ASSERT_FMT(pModule != NULL, "pModule is NULL");
    return pModule;
}

PyObject* rbe_py_cache_get_class(const char* modulePath, const char* classPath) {
//    PyObject* pModule = pyh_cache_get_module(modulePath);
    return NULL;
}
