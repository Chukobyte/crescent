#pragma once

#include <stdbool.h>

bool sf_initialize_simple(const char* title, int windowWidth, int windowHeight);
bool sf_initialize(const char* title,
                   int windowWidth,
                   int windowHeight,
                   int resolutionWidth,
                   int resolutionHeight,
                   const char* controllerDBFilePath);
void sf_process_inputs();
void sf_update();
void sf_render();
bool sf_is_running();
void sf_shutdown();


