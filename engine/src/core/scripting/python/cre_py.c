#include "cre_py.h"

#include <Python.h>

#include "../seika/src/asset/asset_file_loader.h"
#include "../seika/src/utils/se_assert.h"

#include "py_cache.h"
#include "cre_py_api_module.h"
#include "crescent_api_source.h"
#include "../../engine_context.h"

void cre_py_initialize() {
    cre_py_cache_initialize();
    Py_SetProgramName(L"crescent_engine_python");
    PyImport_AppendInittab("crescent_api_internal", &PyInit_cre_py_API); // Load engine modules
    Py_Initialize();
    PyRun_SimpleString("import sys");
    if (sf_asset_file_loader_get_read_mode() == SEAssetFileLoaderReadMode_ARCHIVE) {
        PyRun_SimpleString("from pathlib import PurePath");
        CREEngineContext* engineContext = cre_engine_context_get();
        char sysPathInsertBuffer[256];
        strcpy(sysPathInsertBuffer, "sys.path.insert(0, PurePath(r\'");
        strcat(sysPathInsertBuffer, engineContext->projectArchivePath);
        strcat(sysPathInsertBuffer, "\').as_posix())");
        PyRun_SimpleString(sysPathInsertBuffer);
    } else if (sf_asset_file_loader_get_read_mode() == SEAssetFileLoaderReadMode_DISK) {
        PyRun_SimpleString("sys.path.insert(0, \".\")");
    }
    PyRun_SimpleString("sys.dont_write_bytecode = True");
    // TODO: Not sure why it breaks on linux when not enabling tracemalloc
//#ifndef WIN32
    PyRun_SimpleString("import tracemalloc\n");
    PyRun_SimpleString("tracemalloc.start()");
//#endif

    PyRun_SimpleString(RBE_PY_API_SOURCE_IMPORTER);
    PyRun_SimpleString(RBE_PY_API_SOURCE_IMPORTER_MODULE_IMPORTS);
}

void cre_py_finalize() {
    cre_py_cache_finalize();
//    Py_Finalize(); // FIXME: Figure out why this crashes...
}
