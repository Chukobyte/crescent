#include "editor_context.h"

EditorContext::EditorContext(singleton) {}

std::string EditorContext::GetEngineBinaryPath() const {
    return initialDir + "/bin/crescent_engine.exe";
}

float EditorContext::Time() {
    return SDL_GetTicks() / 1000.0f;
}
