#include "editor_context.h"
#include "utils/file_system_helper.h"

EditorContext::EditorContext(singleton) {}

std::string EditorContext::GetEngineBinPath() const {
    return initialDir + "/bin";
}

std::string EditorContext::GetEngineBinaryPath() const {
    return GetEngineBinPath() + "/" + EDITOR_ENGINE_BINARY_NAME;
}

std::string EditorContext::GetEngineBinaryProgramArgs() const {
    return "-l " + settings.GetGameLogLevelString() + " -ia " + GetEngineBinPath() + "/";
}

std::string EditorContext::GetProjectTempPath() const {
    return initialDir;
}

float EditorContext::Time() {
    return SDL_GetTicks() / 1000.0f;
}
