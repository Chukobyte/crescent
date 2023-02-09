#include "editor_context.h"
#include "utils/file_system_helper.h"

#ifdef _WINDOWS
#define CRESCENT_OS_BIN_DIR "windows"
#elif __APPLE__
#define CRESCENT_OS_BIN_DIR "macosx"
#else
#define CRESCENT_OS_BIN_DIR "linux"
#endif

EditorContext::EditorContext(singleton) {}

std::string EditorContext::GetEngineBinPath() const {
    return initialDir + "/bin/" + CRESCENT_OS_BIN_DIR;
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
    return (float) SDL_GetTicks() / 1000.0f;
}
