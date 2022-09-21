#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

void rbe_py_initialize();
void rbe_py_finalize();
bool rbe_py_load_project_config();

#ifdef __cplusplus
}
#endif
