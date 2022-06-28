#pragma once

#include <stdbool.h>

#include <SDL2/SDL.h>

bool rbe_input_initialize();

void rbe_input_finalize();

void rbe_input_process(SDL_Event event);

void rbe_input_add_action_value(const char* actionName, const char* actionValue, int deviceId);

void rbe_input_remove_action_value(const char* actionName, const char* actionValue);

void rbe_input_remove_action(const char* actionName);

void rbe_input_clean_up_flags();
// Queries
bool rbe_input_is_action_pressed(const char* actionName);

bool rbe_input_is_action_just_pressed(const char* actionName);

bool rbe_input_is_action_just_released(const char* actionName);
