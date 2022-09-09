#pragma once

#include <vector>
#include <functional>

#include "utils/singleton.h"

class SceneNode;

class EditorCallbacks : public Singleton<EditorCallbacks> {
  public:
    EditorCallbacks(singleton) {}

    void RegisterOnSceneNodeSelected(const std::function<void(SceneNode*)>& func);
    void BroadcastOnSceneNodeSelected(SceneNode* sceneNode);

  private:
    std::vector<std::function<void(SceneNode*)>> onSceneNodeSelectedFuncs;
};
