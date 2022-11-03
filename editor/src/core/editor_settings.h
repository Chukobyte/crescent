#pragma once

#include <string>
#include <vector>

#include "../seika/src/utils/logger.h"

#define EDITOR_SETTINGS_FILE_NAME "editor_settings.json"

struct RecentlyLoadedProjectData {
    std::string name;
    std::string fullPath;
};

class EditorSettings {
  public:
    bool LoadSettings();
    void SaveSettings() const;
    void AddToRecentlyLoadedProjectsList(const std::string& name, const std::string& path);
    void SetEditorLogLevel(LogLevel level);
    void SetEditorLogLevel(const std::string& level);
    void SetGameLogLevel(LogLevel level);
    void SetGameLogLevel(const std::string& level);
    [[nodiscard]] LogLevel GetEditorLogLevel() const {
        return editorLogLevel;
    }
    [[nodiscard]] std::string GetEditorLogLevelString() const {
        return se_logger_get_log_level_string(editorLogLevel);
    }
    [[nodiscard]] LogLevel GetGameLogLevel() const {
        return gameLogLevel;
    }
    [[nodiscard]] std::string GetGameLogLevelString() const {
        return se_logger_get_log_level_string(gameLogLevel);
    }

    size_t recentlyLoadedProjectsLimit = 5;
    std::vector<RecentlyLoadedProjectData> recentlyLoadedProjects;

  private:
    LogLevel editorLogLevel = LogLevel_ERROR;
    LogLevel gameLogLevel = LogLevel_ERROR;
};
