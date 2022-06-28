#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

void rbe_py_initialize();

void rbe_py_finalize();

bool rbe_py_load_project_config();

struct RBEGameProperties rbe_py_read_config_path(const char* filePath);

#ifdef __cplusplus
}
#endif
