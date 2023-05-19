#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

typedef enum CrePythonHandleErrorType {
    CrePythonHandleErrorType_FATAL = 0,
    CrePythonHandleErrorType_NON_FATAL = 1,
} CrePythonHandleErrorType;

void cre_py_initialize(const char* embeddedPythonPath);
void cre_py_finalize();
bool cre_py_handle_if_errors(const char* context, CrePythonHandleErrorType errorType);
void cre_py_import_module_source(const char* moduleName, const char* moduleText);
// Exports game, will probably put in another place as this is needed by the editor and abstracts python usage
void cre_py_export_game_project(const char* gameTitle, const char* archivePath, const char* engineBinPath, const char* projectPath, const char* tempPath);

#ifdef __cplusplus
}
#endif
