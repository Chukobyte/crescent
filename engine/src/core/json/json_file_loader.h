#pragma once

// TODO: Decide if we want a callback function to load scene files...
//typedef void (*OnSceneNodeLoad) (SceneNode*);

struct RBEGameProperties* cre_json_load_config_file(const char* filePath);
void cre_json_load_scene_file(const char* filePath);
