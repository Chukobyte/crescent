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
    LogLevel GetEditorLogLevel() const { return editorLogLevel; }
    LogLevel GetGameLogLevel() const { return gameLogLevel; }

    size_t recentlyLoadedProjectsLimit = 5;
    std::vector<RecentlyLoadedProjectData> recentlyLoadedProjects;

private:
    LogLevel editorLogLevel = LogLevel_ERROR;
    LogLevel gameLogLevel = LogLevel_ERROR;
};
