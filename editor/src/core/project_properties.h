#pragma once

#include <string>

#include "../engine/src/core/game_properties.h"

#include "utils/singleton.h"

class ProjectProperties : public Singleton<ProjectProperties> {
  public:
    std::string gameTitle;
    int windowWidth;
    int windowHeight;
    int targetFPS;

    ProjectProperties(singleton);
    ~ProjectProperties();

    void LoadPropertiesFromConfig(const char* modulePath);
    void PrintProperties() const;
};
