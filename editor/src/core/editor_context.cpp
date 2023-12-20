#include "editor_context.h"

#include "../engine/src/core/game_properties.h"

#include "utils/file_system_helper.h"

EditorContext::EditorContext(singleton) {}

std::string EditorContext::GetEngineBinRoot() const {
    return initialDir.empty() ? "bin/" : initialDir + "/bin/";
}

std::string EditorContext::GetEngineBinPath() const {
    return GetEngineBinRoot() + CRESCENT_OS_BIN_DIR;
}

std::string EditorContext::GetEngineBinPathByOS(const std::string &osType) const {
    if (osType == "windows" || osType == "macosx" || osType == "linux") {
        return GetEngineBinRoot() + osType;
    }
    se_logger_error("osType '%s' is invalid!", osType.c_str());
    return {};
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

std::string EditorContext::GetEngineDefaultFontPath() const {
    return GetEngineBinPath() + "/" + CRE_DEFAULT_FONT_ASSET.file_path;
}
