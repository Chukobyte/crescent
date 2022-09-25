#pragma once

#include <string>

#include "../project_properties.h"

#define CRE_CONFIG_FILE_NAME "project.ccfg"

namespace ConfigFileCreator {
void GenerateConfigFile(ProjectProperties* properties, const char* filePath = CRE_CONFIG_FILE_NAME);
}
