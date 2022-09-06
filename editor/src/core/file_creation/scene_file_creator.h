#pragma once

struct SceneNodeFile;

namespace SceneFileCreator {
void GenerateSceneFile(SceneNodeFile* nodeFile, const char* filePath);
}