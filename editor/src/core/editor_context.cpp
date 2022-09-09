#include "editor_context.h"

EditorContext::EditorContext(singleton) {}

float EditorContext::Time() {
    return SDL_GetTicks() / 1000.0f;
}
