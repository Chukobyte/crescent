#include "input_action.h"

#include <SDL2/SDL.h>

#include "../memory/rbe_mem.h"

InputAction* rbe_input_action_create_new_input_action(const char* actionName, int deviceId) {
    InputAction* inputAction = RBE_MEM_ALLOCATE(InputAction);
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
