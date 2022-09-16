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
} // namespace

bool EditorSettings::LoadSettings() {
    const std::string saveFilePath = GetEditorSettingSaveFilePath();
    if (!FileSystemHelper::DoesFileExist(saveFilePath)) {
        rbe_logger_debug("Settings doesn't exist at path '%s', skipping...", saveFilePath.c_str());
        return false;
    }
    nlohmann::json settingsJson = JsonHelper::LoadFile(saveFilePath);
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
