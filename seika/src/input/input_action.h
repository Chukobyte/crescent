#pragma once

#include <stddef.h>
#include <stdbool.h>

#define SE_MAX_INPUT_ACTIONS 32
#define SE_MAX_INPUT_VALUES 4

typedef struct SEInputAction {
    size_t keyboardValueCount;
    size_t mouseValueCount;
    size_t gamepadValueCount;
    int keyboardValues[SE_MAX_INPUT_VALUES];
    char* mouseValues[SE_MAX_INPUT_VALUES];
    char* gamepadValues[SE_MAX_INPUT_VALUES];
    int lastScancodePressed;
    bool isActionPressed;
    bool isActionJustPressed;
    bool isActionJustReleased;
    int deviceId;
} SEInputAction;

SEInputAction* se_input_action_create_new_input_action(const char* actionName, int deviceId);
