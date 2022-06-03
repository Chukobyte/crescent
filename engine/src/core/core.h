#pragma once

#include <stdbool.h>

#define RBE_CORE_VERSION "0.0.1"

bool rbe_initialize(int argv, char** args);
void rbe_update();
bool rbe_is_running();
void rbe_shutdown();