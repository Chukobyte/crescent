#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../../game_properties.h"
#include <stdbool.h>

void rbe_py_initialize();
void rbe_py_finalize();
bool rbe_py_load_project_config();
RBEGameProperties rbe_py_read_config_path(const char* filePath);

#ifdef __cplusplus
}
#endif
