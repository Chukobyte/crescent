#include "rbe_py.h"

#include <Python.h>

#include "py_helper.h"
#include "rbe_py_api_module.h"
#include "../../utils/rbe_assert.h"
#include "../../game_properties.h"

void rbe_py_initialize() {
    pyh_cache_initialize();
    Py_SetProgramName(L"roll_back_engine_python");
    PyImport_AppendInittab("rbe_py_api", &PyInit_rbe_py_API); // Load engine modules
    Py_Initialize();
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.insert(0, \".\")");
    PyRun_SimpleString("sys.dont_write_bytecode = True");
}

void rbe_py_finalize() {
    Py_Finalize();
    pyh_cache_finalize();
}

bool rbe_py_load_project_config() {
    return pyh_run_python_file("rbe_config.py");
}

RBEGameProperties rbe_py_read_config_path(const char* filePath) {
    bool load_success = pyh_run_python_file(filePath);
    RBE_ASSERT_FMT(load_success == true, "Failed to load config at '%s'", filePath);
    RBEGameProperties* gameProps = rbe_game_props_get();
    RBEGameProperties props = {
        .gameTitle = {0},
        .resolutionWidth = gameProps->resolutionWidth,
        .resolutionHeight = gameProps->resolutionHeight,
        .windowWidth = gameProps->windowWidth,
        .windowHeight = gameProps->windowHeight,
        .targetFPS = gameProps->targetFPS
    };
    strcpy(props.gameTitle, gameProps->gameTitle);
    return props;
}
