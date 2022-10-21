#pragma once

#include <stdbool.h>

bool cre_initialize(int argv, char** args);
void cre_update();
bool cre_is_running();
void cre_shutdown();
