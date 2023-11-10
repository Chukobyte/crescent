#include "input_action.h"

#include "SDL2/SDL.h"

#include "../memory/se_mem.h"

SEInputAction* se_input_action_create_new_input_action(const char* actionName, int deviceId) {
    SEInputAction* inputAction = SE_MEM_ALLOCATE(SEInputAction);
    inputAction->keyboardValueCount = 0;
    inputAction->mouseValueCount = 0;
    inputAction->gamepadValueCount = 0;
    inputAction->lastScancodePressed = SDL_SCANCODE_UNKNOWN;
    inputAction->isActionPressed = false;
    inputAction->isActionJustPressed = false;
    inputAction->isActionJustReleased = false;
    inputAction->deviceId = deviceId;
    return inputAction;
}
