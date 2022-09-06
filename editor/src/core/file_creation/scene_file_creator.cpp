#include "scene_file_creator.h"

#include "../scene/scene_manager.h"
#include "../utils/file_system_helper.h"

void SceneFileCreator::GenerateSceneFile(SceneNodeFile *nodeFile, const char *filePath) {
    std::string fileContents = "from crescent_api import *\n";
    fileContents += "\n";
    // Begin
    fileContents += "create_stage_nodes(\n";
    fileContents += "    stage_nodes=[\n";
    // Stage Nodes
    fileContents += "        StageNode(\n";
    fileContents += "            name=\"Main\",\n";
    fileContents += "            type=\"Node2D\",\n";
    fileContents += "            tags=None,\n";
    fileContents += "            external_node_source=None,\n";
    fileContents += "            components=[],\n";
    fileContents += "            children=[],\n";
    fileContents += "        ),\n";
    // End
    fileContents += "    ],\n";
    fileContents += ")\n";

    FileSystemHelper::WriteFile(filePath, fileContents);
}
