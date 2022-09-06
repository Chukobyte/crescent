#include "scene_file_creator.h"

#include "../scene/scene_manager.h"
#include "../utils/file_system_helper.h"

namespace {
std::string GetStageNodeText(SceneNode* sceneNode, int tabSpace = 0) {
    const std::string tabSpaceText = std::string(tabSpace, ' ');
    std::string fileContents;
    fileContents += tabSpaceText + "        StageNode(\n";
    fileContents += tabSpaceText + "            name=\"Main\",\n";
    fileContents += tabSpaceText + "            type=\"Node2D\",\n";
    fileContents += tabSpaceText + "            tags=None,\n";
    fileContents += tabSpaceText + "            external_node_source=None,\n";
    fileContents += tabSpaceText + "            components=[],\n";
    fileContents += tabSpaceText + "            children=[],\n";
    fileContents += tabSpaceText + "        ),\n";
    return fileContents;
}
} // namespace

void SceneFileCreator::GenerateSceneFile(SceneNodeFile *nodeFile, const char *filePath) {
    std::string fileContents = "from crescent_api import *\n";
    fileContents += "\n";
    // Begin
    fileContents += "create_stage_nodes(\n";
    fileContents += "    stage_nodes=[\n";
    // Stage Nodes
    fileContents += GetStageNodeText(nodeFile->rootNode);
    // End
    fileContents += "    ],\n";
    fileContents += ")\n";

    FileSystemHelper::WriteFile(filePath, fileContents);
}
