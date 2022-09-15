#pragma once

#include <string>
#include <vector>

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

    size_t recentlyLoadedProjectsLimit = 5;
    std::vector<RecentlyLoadedProjectData> recentlyLoadedProjects;
};
