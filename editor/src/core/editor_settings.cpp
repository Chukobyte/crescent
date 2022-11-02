#include "editor_settings.h"

#include <SDL2/SDL.h>

#include "utils/file_system_helper.h"
#include "utils/json_helper.h"

namespace {
std::string GetEditorSettingsSavePath() {
    char* appDataPath = SDL_GetPrefPath("crescent", "editor");
    std::string editorSettingsSavePath = appDataPath;
    SDL_free(appDataPath);
    return editorSettingsSavePath;
}

std::string GetEditorSettingSaveFilePath() {
    static const std::string editorSettingsSavePath = GetEditorSettingsSavePath();
    return editorSettingsSavePath + "/" + EDITOR_SETTINGS_FILE_NAME;
}

std::string GetLogLevelString(LogLevel level) {
    switch (level) {
        case LogLevel_ERROR:
            return "error";
            break;
        case LogLevel_WARN:
            return "warn";
            break;
        case LogLevel_INFO:
            return "info";
            break;
        case LogLevel_DEBUG:
            return "debug";
            break;
    }
    return "unknown";
}

LogLevel GetLogLevelEnum(const std::string& level, LogLevel failValue = LogLevel_ERROR) {
    if (level == "error") {
        return LogLevel_ERROR;
    } else if (level == "warn") {
        return LogLevel_WARN;
    } else if (level == "info") {
        return LogLevel_INFO;
    } else if (level == "debug") {
        return LogLevel_DEBUG;
    }
    return failValue;
}
} // namespace

bool EditorSettings::LoadSettings() {
    const std::string saveFilePath = GetEditorSettingSaveFilePath();
    if (!FileSystemHelper::DoesFileExist(saveFilePath)) {
        se_logger_debug("Settings doesn't exist at path '%s', skipping...", saveFilePath.c_str());
        return false;
    }
    nlohmann::json settingsJson = JsonHelper::LoadFile(saveFilePath);
    // Log levels
    const std::string editorLogLevelString = JsonHelper::GetDefault<std::string>(settingsJson, "editor_log_level", GetLogLevelString(editorLogLevel));
    editorLogLevel = GetLogLevelEnum(editorLogLevelString);
    const std::string gameLogLevelString = JsonHelper::GetDefault<std::string>(settingsJson, "game_log_level", GetLogLevelString(gameLogLevel));
    gameLogLevel = GetLogLevelEnum(gameLogLevelString);
    // Recently loaded projects
    auto recentlyLoadedProjectsJson = JsonHelper::Get<nlohmann::ordered_json>(settingsJson, "recently_loaded_projects");
    for (auto& projectJson : recentlyLoadedProjectsJson) {
        RecentlyLoadedProjectData loadedProjectData = {
            JsonHelper::Get<std::string>(projectJson, "name"),
            JsonHelper::Get<std::string>(projectJson, "path")
        };
        recentlyLoadedProjects.emplace_back(loadedProjectData);
    }
    return true;
}

void EditorSettings::SaveSettings() const {
    nlohmann::ordered_json settingsJson;
    // Log levels
    settingsJson["editor_log_level"] = GetLogLevelString(editorLogLevel);
    settingsJson["game_log_level"] = GetLogLevelString(gameLogLevel);
    // Recently loaded projects
    nlohmann::ordered_json recentlyLoadedProjectsJsonArray = nlohmann::ordered_json::array();
    for (const auto& project : recentlyLoadedProjects) {
        nlohmann::ordered_json projectJson;
        projectJson["name"] = project.name;
        projectJson["path"] = project.fullPath;
        recentlyLoadedProjectsJsonArray.emplace_back(projectJson);
    }
    settingsJson["recently_loaded_projects"] = recentlyLoadedProjectsJsonArray;
    JsonHelper::SaveFile(GetEditorSettingSaveFilePath(), settingsJson);
}

void EditorSettings::AddToRecentlyLoadedProjectsList(const std::string& name, const std::string& path) {
    // Temp exclusion for 'Fighter Proto'
    if (name == "Fighter Proto") {
        return;
    }
    static auto HasProjectNameInList = [this] (const std::string& projectName) {
        for (const auto& project : recentlyLoadedProjects) {
            if (project.name == projectName) {
                return true;
            }
        }
        return false;
    };
    if (!HasProjectNameInList(name)) {
        recentlyLoadedProjects.emplace_back(RecentlyLoadedProjectData{ name, path });
    }
}

void EditorSettings::SetEditorLogLevel(LogLevel level) {
    editorLogLevel = level;
    se_logger_set_level(editorLogLevel);
}

void EditorSettings::SetEditorLogLevel(const std::string& level) {
    SetEditorLogLevel(GetLogLevelEnum(level));
}

void EditorSettings::SetGameLogLevel(LogLevel level) {
    gameLogLevel = level;
}

void EditorSettings::SetGameLogLevel(const std::string& level) {
    SetGameLogLevel(GetLogLevelEnum(level));
}
