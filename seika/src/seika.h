#pragma once

#include <stdbool.h>

bool sf_initialize(const char* title, int windowWidth, int windowHeight);
void sf_process_inputs();
void sf_update();
void sf_render();
bool sf_is_running();
void sf_shutdown();


