#pragma once

#include <stdbool.h>
#include <stdint.h>

bool sf_initialize_simple(const char* title, int windowWidth, int windowHeight);
bool sf_initialize(const char* title,
                   int windowWidth,
                   int windowHeight,
                   int resolutionWidth,
                   int resolutionHeight,
                   uint32_t audioWavSampleRate,
                   bool maintainAspectRatio,
                   const char* controllerDBFilePath);
void sf_process_inputs();
void sf_fixed_update(float deltaTime);
void sf_render();
bool sf_is_running();
void sf_shutdown();


