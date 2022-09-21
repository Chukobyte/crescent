#include "editor_callbacks.h"

#include "scene/scene_manager.h"

void EditorCallbacks::RegisterOnSceneNodeSelected(const std::function<void(SceneNode*)>& func) {
    onSceneNodeSelectedFuncs.emplace_back(func);
}

void EditorCallbacks::BroadcastOnSceneNodeSelected(SceneNode *sceneNode) {
    for (auto& func : onSceneNodeSelectedFuncs) {
        func(sceneNode);
    }
}
