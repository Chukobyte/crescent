#pragma once

#include <stdbool.h>

#include <SDL2/SDL.h>

bool se_input_initialize();
void se_input_finalize();
void se_input_process(SDL_Event event);
void se_input_add_action_value(const char* actionName, const char* actionValue, int deviceId);
void se_input_remove_action_value(const char* actionName, const char* actionValue);
void se_input_remove_action(const char* actionName);
void se_input_clean_up_flags();
// Queries
bool se_input_is_action_pressed(const char* actionName);
bool se_input_is_action_just_pressed(const char* actionName);
bool se_input_is_action_just_released(const char* actionName);
// Gamepad
void se_input_gamepad_start_vibration(int device, float weakMagnitude, float strongMagnitude, float durationSeconds);
void se_input_gamepad_stop_vibration(int device);
