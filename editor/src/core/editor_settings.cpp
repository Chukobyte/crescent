#include "editor_settings.h"

#include <SDL3/SDL.h>

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
} // namespace

bool EditorSettings::Load() {
    const std::string saveFilePath = GetEditorSettingSaveFilePath();
    if (!FileSystemHelper::DoesFileExist(saveFilePath)) {
        ska_logger_debug("Settings doesn't exist at path '%s', skipping...", saveFilePath.c_str());
        return false;
    }
    nlohmann::json settingsJson = JsonHelper::LoadFile(saveFilePath);
    // Log levels
    const std::string editorLogLevelString = JsonHelper::GetDefault<std::string>(settingsJson, "editor_log_level", GetEditorLogLevelString());
    SetEditorLogLevel(editorLogLevelString);
    const std::string gameLogLevelString = JsonHelper::GetDefault<std::string>(settingsJson, "game_log_level", GetGameLogLevelString());
    SetGameLogLevel(gameLogLevelString);
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

void EditorSettings::Save() const {
    nlohmann::ordered_json settingsJson;
    // Log levels
    settingsJson["editor_log_level"] = GetEditorLogLevelString();
    settingsJson["game_log_level"] = GetGameLogLevelString();
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

void EditorSettings::SetEditorLogLevel(SkaLogLevel level) {
    editorLogLevel = level;
    ska_logger_set_level(editorLogLevel);
}

void EditorSettings::SetEditorLogLevel(const std::string& level) {
    const auto logLevel = ska_logger_get_log_level_enum(level.c_str());
    SetEditorLogLevel(logLevel);
}

void EditorSettings::SetGameLogLevel(SkaLogLevel level) {
    gameLogLevel = level;
}

void EditorSettings::SetGameLogLevel(const std::string& level) {
    const auto logLevel = ska_logger_get_log_level_enum(level.c_str());
    SetGameLogLevel(logLevel);
}
