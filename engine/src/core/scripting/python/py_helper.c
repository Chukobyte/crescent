#include "py_helper.h"

#include <Python.h>

#include "../../utils/rbe_assert.h"
#include "../../data_structures/rbe_hash_map_string.h"

// Helper functions
const char* pyh_get_string_from_obj(PyObject* obj) {
    RBE_ASSERT_FMT(PyUnicode_Check(obj), "Not a valid string!");
    PyObject* pRepr = PyObject_Str(obj);
    RBE_ASSERT(pRepr != NULL);
    PyObject* pStr = PyUnicode_AsEncodedString(pRepr, "utf-8", "~E~");
    RBE_ASSERT(pStr != NULL);
    const char* str = PyBytes_AsString(pStr);
    Py_DecRef(pRepr);
    Py_DecRef(pStr);
    return str;
}

const char* pyh_get_string_from_dict(PyObject* pyDict, const char* key) {
    PyObject* pStr = PyDict_GetItemString(pyDict, key);
    RBE_ASSERT(pStr != NULL);
    const char* str = pyh_get_string_from_obj(pStr);
    Py_DecRef(pStr);
    return str;
}

const char* pyh_get_string_from_dict_default(PyObject* pyDict, const char* key, const char* defaultValue) {
    PyObject* pKey = PyUnicode_FromString(key);
    PyObject* pDefaultValue = PyUnicode_FromString(defaultValue);

    PyObject* pStr = PyDict_SetDefault(pyDict, pKey, pDefaultValue);
    RBE_ASSERT(pStr != NULL);
    const char* str = pyh_get_string_from_obj(pStr);
    Py_DecRef(pKey);
    Py_DecRef(pDefaultValue);
    Py_DecRef(pStr);
    return str;
}

int pyh_get_int_from_dict(PyObject* pyDict, const char* key) {
    PyObject* pInt = PyDict_GetItemString(pyDict, key);
    RBE_ASSERT(pInt != NULL);
    RBE_ASSERT(PyLong_Check(pInt));
    const int intResult = PyLong_AsLong(pInt);
    Py_DecRef(pInt);
    return intResult;
}

int pyh_get_int_from_dict_default(PyObject* pyDict, const char* key, int defaultValue) {
    PyObject* pKey = PyUnicode_FromString(key);
    PyObject* pDefaultValue = PyLong_FromLong(defaultValue);
    PyObject* pInt = PyDict_SetDefault(pyDict, pKey, pDefaultValue);
    RBE_ASSERT(pInt != NULL);
    RBE_ASSERT(PyLong_Check(pInt));
    const int intResult = PyLong_AsLong(pInt);
    Py_DecRef(pKey);
    Py_DecRef(pDefaultValue);
    Py_DecRef(pInt);
    return intResult;
}

bool pyh_run_python_file(const char* filePath) {
    PyObject* pFilePath = Py_BuildValue("s", filePath);
    FILE* configFile = _Py_fopen_obj(pFilePath, "r+");
    if (configFile != NULL) {
        int result = PyRun_SimpleFile(configFile, filePath);
        Py_DECREF(pFilePath);
        return (result == 0);
    }
    Py_DECREF(pFilePath);
    return false;
}

const char* phy_get_string_from_var(struct _object* obj, const char* variableName) {
    RBE_ASSERT(obj != NULL);
    Py_IncRef(obj);
    PyObject* objVar = PyObject_GetAttrString(obj, variableName);
    RBE_ASSERT(objVar != NULL);
    const char* varString = PyUnicode_AsUTF8(objVar);
    Py_DecRef(objVar);
    return varString;
}

int phy_get_int_from_var(struct _object* obj, const char* variableName) {
    RBE_ASSERT(obj != NULL);
    Py_IncRef(obj);
    PyObject* objVar = PyObject_GetAttrString(obj, variableName);
    RBE_ASSERT(objVar != NULL);
    int varInt = PyLong_AsLong(objVar);
    Py_DecRef(objVar);
    return varInt;
}

// Python Cache
#define PY_REF_CLEANER_OBJ_MAX 32

typedef struct PyRefCountCleaner {
    PyObject* objects[PY_REF_CLEANER_OBJ_MAX];
    int count;
} PyRefCountCleaner;

static PyRefCountCleaner refCountCleaner;
static RBEStringHashMap * pyCacheHashMap = NULL;

void pyh_cache_initialize() {
    for (int i = 0; i < PY_REF_CLEANER_OBJ_MAX; i++) {
        refCountCleaner.objects[i] = NULL;
    }
    refCountCleaner.count = 0;
    pyCacheHashMap = rbe_string_hash_map_create(128);
}

void pyh_cache_finalize() {
    pyh_ref_clean();
    rbe_string_hash_map_destroy(pyCacheHashMap);
}

PyObject* pyh_cache_get_module(const char* modulePath) {
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

PyObject* pyh_ref_count(PyObject* pyObject) {
    refCountCleaner.objects[refCountCleaner.count] = pyObject;
    refCountCleaner.count++;
    return pyObject;
}

void pyh_ref_clean() {
    for (int i = 0; i < refCountCleaner.count; i++) {
        Py_DecRef(refCountCleaner.objects[i]);
        refCountCleaner.objects[i] = NULL;
    }
    refCountCleaner.count = 0;
}
