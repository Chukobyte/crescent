#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include "../../game_properties.h"

void rbe_py_initialize();
void rbe_py_finalize();
bool rbe_py_load_project_config();
RBEGameProperties* rbe_py_load_game_properties2(const char* filePath);

#ifdef __cplusplus
}
#endif
