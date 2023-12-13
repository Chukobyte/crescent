#include "cre_py.h"

#include <Python.h>

#include <seika/asset/asset_file_loader.h>
#include <seika/memory/se_mem.h>
#include <seika/utils/se_file_system_utils.h>
#include <seika/utils/se_string_util.h>
#include <seika/utils/se_assert.h>

#include "py_cache.h"
#include "cre_py_api_module.h"
#include "crescent_api_source.h"
#include "../../engine_context.h"

void cre_py_set_python_home(const char* embeddedPythonPath);

void cre_py_initialize(const char* embeddedPythonPath) {
    if (embeddedPythonPath != NULL) {
        cre_py_set_python_home(embeddedPythonPath);
    }
    // Initialize python
    cre_py_cache_initialize();
    Py_SetProgramName(L"crescent_engine_python");
    PyImport_AppendInittab("crescent_api_internal", &PyInit_cre_py_API); // Load engine modules
    Py_Initialize();
    PyRun_SimpleString("import sys");
    // Set first path for python module lookups
    if (sf_asset_file_loader_get_read_mode() == SEAssetFileLoaderReadMode_ARCHIVE) {
        PyRun_SimpleString("from pathlib import PurePath");
        CREEngineContext* engineContext = cre_engine_context_get();
        char sysPathInsertBuffer[256];
        se_strcpy(sysPathInsertBuffer, "sys.path.insert(0, PurePath(r\'");
        se_strcat(sysPathInsertBuffer, engineContext->projectArchivePath);
        se_strcat(sysPathInsertBuffer, "\').as_posix())");
        PyRun_SimpleString(sysPathInsertBuffer);
    } else if (sf_asset_file_loader_get_read_mode() == SEAssetFileLoaderReadMode_DISK) {
        PyRun_SimpleString("sys.path.insert(0, \".\")");
    }
    // Disable writing compiled python files if they are imported as python files
    PyRun_SimpleString("sys.dont_write_bytecode = True"); // TODO: Make this an advanced setting within the editor
    // Enable tracemalloc for more information on stack traces
    PyRun_SimpleString("import tracemalloc\n");
    PyRun_SimpleString("tracemalloc.start()");
    // Import custom python source importer
    PyRun_SimpleString(RBE_PY_API_SOURCE_IMPORTER);
    // Import other custom python modules
    cre_py_import_module_source("crescent_api", RBE_PY_API_SOURCE);
}

void cre_py_finalize() {
    cre_py_cache_finalize();
    Py_Finalize();
}

bool cre_py_handle_if_errors(const char* context, CrePythonHandleErrorType errorType) {
    if (PyErr_Occurred()) {
        // Get the exception type, value, and traceback objects
        PyObject* excType = NULL;
        PyObject* excValue = NULL;
        PyObject* excTraceback = NULL;
        PyErr_Fetch(&excType, &excValue, &excTraceback);

        // Convert the exception objects to strings
        const char *typeStr = PyUnicode_AsUTF8(PyObject_Str(excType));
        const char *valueStr = PyUnicode_AsUTF8(PyObject_Str(excValue));

        // Format the error message
        char errorMessage[256];
        snprintf(errorMessage, sizeof(errorMessage), "Python error: %s: %s", typeStr, valueStr);

        // Print or log the error message
        se_logger_error(errorMessage);

        PyErr_Clear();

        if (errorType == CrePythonHandleErrorType_FATAL) {
            SE_ASSERT_FMT(false, "Fatal python error, context: '%s'", context);
        }

        return true;
    }
    return false;
}

void cre_py_import_module_source(const char* moduleName, const char* moduleText) {
#define IMPORT_COMMAND_BUFFER_SIZE 65536
    char importCommandBuffer[IMPORT_COMMAND_BUFFER_SIZE];
    se_strcpy(importCommandBuffer, "from source_importer import SourceImporter\n");
    se_strcat(importCommandBuffer, "source_code_to_import = \"\"\"\n");
    se_strcat(importCommandBuffer, moduleText);
    se_strcat(importCommandBuffer, "\"\"\"\n");
    se_strcat(importCommandBuffer, "SourceImporter.import_from_source(\"");
    se_strcat(importCommandBuffer, moduleName);
    se_strcat(importCommandBuffer, "\", source_code_to_import)");
    PyRun_SimpleString(importCommandBuffer);
#undef IMPORT_COMMAND_BUFFER_SIZE
}

void cre_py_export_game_project(const char* gameTitle, const char* archivePath, const char* engineBinPath, const char* projectPath, const char* tempPath) {
#define EXPORT_GAME_COMMAND_BUFFER_SIZE 1024
    char exportGameCommandBuffer[EXPORT_GAME_COMMAND_BUFFER_SIZE];
    se_strcpy(exportGameCommandBuffer, "from game_exporter import GameExporter\n");
    se_strcat(exportGameCommandBuffer, "GameExporter.export_game(\n");
    // Game Title
    se_strcat(exportGameCommandBuffer, "   game_title=r\"");
    se_strcat(exportGameCommandBuffer, gameTitle);
    se_strcat(exportGameCommandBuffer, "\",\n");
    // Archive Path
    se_strcat(exportGameCommandBuffer, "   archive_name=r\"");
    se_strcat(exportGameCommandBuffer, archivePath);
    se_strcat(exportGameCommandBuffer, "\",\n");
    // Engine Bin path
    se_strcat(exportGameCommandBuffer, "   engine_bin_dir=r\"");
    se_strcat(exportGameCommandBuffer, engineBinPath);
    se_strcat(exportGameCommandBuffer, "\",\n");
    // Project path
    se_strcat(exportGameCommandBuffer, "   project_dir=r\"");
    se_strcat(exportGameCommandBuffer, projectPath);
    se_strcat(exportGameCommandBuffer, "\",\n");
    // Temp path
    se_strcat(exportGameCommandBuffer, "   temp_path=r\"");
    se_strcat(exportGameCommandBuffer, tempPath);
    se_strcat(exportGameCommandBuffer, "\",\n");

    se_strcat(exportGameCommandBuffer, ")");
    PyRun_SimpleString(exportGameCommandBuffer);
#undef EXPORT_GAME_COMMAND_BUFFER_SIZE
}

void cre_py_set_python_home(const char* embeddedPythonPath) {
    // The string needs to stay in memory for the PYTHONHOME environment variable to keep its value
    static char fullEmbeddedPythonPath[1024];
    snprintf(fullEmbeddedPythonPath, sizeof(fullEmbeddedPythonPath), "%s/embed_python", embeddedPythonPath);
    if (se_fs_does_dir_exist(fullEmbeddedPythonPath)) {
        se_logger_debug("Found embedded python package at '%s'", fullEmbeddedPythonPath);
        static char pythonHomeEnvVar[2048];
        // Set PYTHONHOME
        const char* currentPythonHOME = getenv("PYTHONHOME"); // Seems like we don't need to free pointer?
        if (currentPythonHOME != NULL) {
            snprintf(pythonHomeEnvVar, sizeof(pythonHomeEnvVar), "PYTHONHOME=%s:%s", currentPythonHOME, fullEmbeddedPythonPath);
        } else {
            snprintf(pythonHomeEnvVar, sizeof(pythonHomeEnvVar), "PYTHONHOME=%s", fullEmbeddedPythonPath);
        }
        if (putenv(pythonHomeEnvVar) == 0) {
            se_logger_debug("Has set environment var: '%s'", pythonHomeEnvVar);
        } else {
            se_logger_error("Failed to set environment var: '%s'", pythonHomeEnvVar);
        }
    } else {
        se_logger_debug("Didn't find embedded python package at '%s'", fullEmbeddedPythonPath);
    }
}
