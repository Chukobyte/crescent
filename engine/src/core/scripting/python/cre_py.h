#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

void cre_py_initialize();
void cre_py_finalize();
void cre_py_import_module_source(const char* moduleName, const char* moduleText);

#ifdef __cplusplus
}
#endif
