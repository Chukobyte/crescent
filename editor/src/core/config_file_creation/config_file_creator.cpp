#include "config_file_creator.h"
#include "../utils/helper.h"
//#include "../utils/file_system_helper.h"

void ConfigFileCreator::GenerateConfigFile(const char *filePath, ProjectProperties* properties) {
    std::string fileContents = "from crescent_api import *\n";
    fileContents += "\n";
    fileContents += "configure_game(\n";
    fileContents += "   game_title=\"" + properties->gameTitle + "\",\n";
    fileContents += "   window_width=\"" + std::to_string(properties->windowWidth) + "\",\n";
    fileContents += "   window_height=\"" + std::to_string(properties->windowHeight) + "\",\n";
    fileContents += "   resolution_width=\"" + std::to_string(properties->resolutionWidth) + "\",\n";
    fileContents += "   resolution_height=\"" + std::to_string(properties->resolutionHeight) + "\",\n";
    fileContents += "   target_fps=\"" + std::to_string(properties->targetFPS) + "\",\n";
    fileContents += "   initial_node_path=\"" + properties->initialScenePath + "\",\n";
    fileContents += "   colliders_visible=\"" + Helper::BoolToStringCaptital(properties->areCollidersVisible) + "\",\n";
    fileContents += ")\n";
    fileContents += "\n";

//    FileSystemHelper::WriteFile(filePath, fileContents);
}
