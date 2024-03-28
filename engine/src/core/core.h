#pragma once

#include <stdbool.h>

#include <seika/defines.h>

bool cre_initialize(int32 argv, char** args);
void cre_update();
bool cre_is_running();
int32 cre_shutdown();
