#include "py_cache.h"

#include <Python.h>

#include "../../data_structures/rbe_hash_map_string.h"
#include "../../utils/rbe_assert.h"

typedef struct PyModuleCacheItem {
    PyObject* module;
    PyObject* classes[16];
    size_t classCount;
} PyModuleCacheItem;

typedef struct PyModuleCache {
    PyModuleCacheItem cache[32];
    size_t cacheSize;
} PyModuleCache;

static RBEStringHashMap* pyCacheHashMap = NULL;

void rbe_py_cache_initialize() {
    pyCacheHashMap = rbe_string_hash_map_create(128);
}

void rbe_py_cache_finalize() {
    rbe_string_hash_map_destroy(pyCacheHashMap);
}

PyObject* rbe_py_cache_get_module(const char* modulePath) {
    if (!rbe_string_hash_map_has(pyCacheHashMap, modulePath)) {
        PyObject* pName = PyUnicode_FromString(modulePath);
        PyObject* pNewModule = PyImport_Import(pName);
        RBE_ASSERT(pNewModule != NULL);
        rbe_string_hash_map_add(pyCacheHashMap, modulePath, pNewModule, sizeof(*pNewModule));
        Py_DecRef(pName);
        // Increase ref to store in cache and to return to caller
        Py_IncRef(pNewModule);
    }
    PyObject* pModule = (PyObject*) rbe_string_hash_map_get(pyCacheHashMap, modulePath);
    RBE_ASSERT_FMT(pModule != NULL, "pModule is NULL");
    return pModule;
}

PyObject* rbe_py_cache_get_class(const char* modulePath, const char* classPath) {
//    PyObject* pModule = pyh_cache_get_module(modulePath);
    return NULL;
}
