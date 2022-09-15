#include "editor_context.h"

EditorContext::EditorContext(singleton) {}

std::string EditorContext::GetEngineBinPath() const {
    return initialDir + "/bin";
}

std::string EditorContext::GetEngineBinaryPath() const {
#ifdef _WIN32
    return GetEngineBinPath() + "/crescent_engine.exe";
#else
    return GetEngineBinPath() + "/crescent_engine";
#endif
}

std::string EditorContext::GetEngineBinaryProgramArgs() const {
    return "-ia " + GetEngineBinPath() + "/";
}

std::string EditorContext::GetProjectExportPath() const {
    return initialDir + "/export";
}

float EditorContext::Time() {
    return SDL_GetTicks() / 1000.0f;
}
