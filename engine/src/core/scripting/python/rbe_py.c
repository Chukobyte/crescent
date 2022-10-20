#include "rbe_py.h"

#include <Python.h>

#include "../seika/src/utils/rbe_assert.h"

#include "py_cache.h"
#include "rbe_py_api_module.h"
#include "crescent_api_source.h"

void rbe_py_initialize() {
    rbe_py_cache_initialize();
    Py_SetProgramName(L"crescent_engine_python");
    PyImport_AppendInittab("crescent_api_internal", &PyInit_rbe_py_API); // Load engine modules
    Py_Initialize();
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.insert(0, \".\")");
    PyRun_SimpleString("sys.dont_write_bytecode = True");
    // TODO: Not sure why it breaks on linux when not enabling tracemalloc
//#ifndef WIN32
    PyRun_SimpleString("import tracemalloc\n");
    PyRun_SimpleString("tracemalloc.start()");
//#endif

    PyRun_SimpleString(RBE_PY_API_SOURCE_IMPORTER);
    PyRun_SimpleString(RBE_PY_API_SOURCE_IMPORTER_MODULE_IMPORTS);
}

void rbe_py_finalize() {
    rbe_py_cache_finalize();
//    Py_Finalize(); // FIXME: Figure out why this crashes...
}
