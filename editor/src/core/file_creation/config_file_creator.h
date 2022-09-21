#pragma once

#include <string>

#include "../project_properties.h"

namespace ConfigFileCreator {
void GenerateConfigFile(const char* filePath, ProjectProperties* properties);
}
