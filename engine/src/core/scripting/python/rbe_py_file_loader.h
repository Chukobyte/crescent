#pragma once

#include "../../game_properties.h"
#include "../../scene/scene_manager.h"

RBEGameProperties* rbe_py_load_game_properties(const char* filePath);
SceneTreeNode* rbe_py_load_scene(const char* filePath);
