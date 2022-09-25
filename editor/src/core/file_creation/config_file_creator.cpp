#include "config_file_creator.h"

void ConfigFileCreator::GenerateConfigFile(ProjectProperties* properties, const char* filePath) {
    JsonHelper::SaveFile(filePath, properties->ToJson());
}
