#pragma once

#include <stdbool.h>

#include <SDL2/SDL.h>

bool cre_input_initialize(const char* controllerDBFilePath);
void cre_input_finalize();
void cre_input_process(SDL_Event event);
void cre_input_add_action_value(const char* actionName, const char* actionValue, int deviceId);
void cre_input_remove_action_value(const char* actionName, const char* actionValue);
void cre_input_remove_action(const char* actionName);
void cre_input_clean_up_flags();
// Queries
bool cre_input_is_action_pressed(const char* actionName);
bool cre_input_is_action_just_pressed(const char* actionName);
bool cre_input_is_action_just_released(const char* actionName);
