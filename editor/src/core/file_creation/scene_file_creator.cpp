#include "scene_file_creator.h"

#include "../scene/scene_manager.h"
#include "../utils/file_system_helper.h"

void SceneFileCreator::GenerateSceneFile(SceneNodeFile *nodeFile, const char *filePath) {
    std::string fileContents = "from crescent_api import *\n";

    FileSystemHelper::WriteFile(filePath, fileContents);
}
