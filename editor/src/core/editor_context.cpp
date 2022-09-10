#include "editor_context.h"

EditorContext::EditorContext(singleton) {}

std::string EditorContext::GetEngineBinaryPath() const {
    return initialDir + "/bin/crescent_engine.exe";
}

std::string EditorContext::GetEngineBinaryProgramArgs() const {
    return "-ia " + initialDir + "/bin/";
}

float EditorContext::Time() {
    return SDL_GetTicks() / 1000.0f;
}
