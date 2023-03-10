#pragma once

#define CRE_SCENE_CACHE_MAX_ITEMS 100
#define CRE_SCENE_CACHE_INVALID_ID (-1)

typedef int CreSceneCacheId;

void cre_scene_template_cache_initialize();
void cre_scene_template_cache_finalize();
CreSceneCacheId cre_scene_template_cache_load_scene(const char* scenePath);
struct JsonSceneNode* cre_scene_template_cache_get_scene(CreSceneCacheId cacheId);
