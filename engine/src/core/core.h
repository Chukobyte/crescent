#pragma once

#include <stdbool.h>

bool rbe_initialize(int argv, char** args);
void rbe_update();
bool rbe_is_running();
void rbe_shutdown();