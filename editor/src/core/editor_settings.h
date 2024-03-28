#pragma once

#include <string>
#include <vector>

#include <seika/logger.h>

#define EDITOR_SETTINGS_FILE_NAME "editor_settings.json"

struct RecentlyLoadedProjectData {
    std::string name;
    std::string fullPath;
};

class EditorSettings {
  public:
    bool Load();
    void Save() const;
    void AddToRecentlyLoadedProjectsList(const std::string& name, const std::string& path);
    void SetEditorLogLevel(SkaLogLevel level);
    void SetEditorLogLevel(const std::string& level);
    void SetGameLogLevel(SkaLogLevel level);
    void SetGameLogLevel(const std::string& level);
    [[nodiscard]] SkaLogLevel GetEditorLogLevel() const {
        return editorLogLevel;
    }
    [[nodiscard]] std::string GetEditorLogLevelString() const {
        return ska_logger_get_log_level_string(editorLogLevel);
    }
    [[nodiscard]] SkaLogLevel GetGameLogLevel() const {
        return gameLogLevel;
    }
    [[nodiscard]] std::string GetGameLogLevelString() const {
        return ska_logger_get_log_level_string(gameLogLevel);
    }

    size_t recentlyLoadedProjectsLimit = 5;
    std::vector<RecentlyLoadedProjectData> recentlyLoadedProjects;

  private:
    SkaLogLevel editorLogLevel = SkaLogLevel_ERROR;
    SkaLogLevel gameLogLevel = SkaLogLevel_ERROR;
};
