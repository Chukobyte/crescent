#include "scene_template_cache.h"

#include <string.h>

#include "seika/asset/asset_file_loader.h"
#include "seika/data_structures/se_hash_map_string.h"
#include "seika/utils/se_assert.h"
#include "seika/utils/se_string_util.h"

#include "../json/json_file_loader.h"

typedef struct SECachedSceneTemplate {
    char* path;
    JsonSceneNode* rootNode;
} SECachedSceneTemplate;

static SECachedSceneTemplate cachedSceneTemplates[CRE_SCENE_CACHE_MAX_ITEMS];
CreSceneCacheId numberOfCachedSceneTemplates = 0;

CreSceneCacheId scene_template_get_cache_id(const char* scenePath);

void cre_scene_template_cache_initialize() {}

void cre_scene_template_cache_finalize() {
    for (CreSceneCacheId cacheId = 0; cacheId < numberOfCachedSceneTemplates; cacheId++) {
        if (cachedSceneTemplates[cacheId].path) {
            SE_MEM_FREE(cachedSceneTemplates[cacheId].path);
        }
        if (cachedSceneTemplates[cacheId].rootNode) {
            cre_json_delete_json_scene_node(cachedSceneTemplates[cacheId].rootNode);
        }
    }
    numberOfCachedSceneTemplates = 0;
}

CreSceneCacheId cre_scene_template_cache_load_scene(const char* scenePath) {
    CreSceneCacheId cacheId = CRE_SCENE_CACHE_INVALID_ID;
    char* sceneText = sf_asset_file_loader_read_file_contents_as_string(scenePath, NULL);
    if (sceneText) {
        // Check if we already have the scene cached
        cacheId = scene_template_get_cache_id(scenePath);
        if (cacheId == CRE_SCENE_CACHE_INVALID_ID) {
            // We don't have it cached, so create a new one
            cacheId = numberOfCachedSceneTemplates++;
            cachedSceneTemplates[cacheId].path = se_strdup(scenePath);
            cachedSceneTemplates[cacheId].rootNode = cre_json_load_scene_file(scenePath);
        }
    }
    return cacheId;
}

JsonSceneNode* cre_scene_template_cache_get_scene(CreSceneCacheId cacheId) {
    return cachedSceneTemplates[cacheId].rootNode;
}

CreSceneCacheId scene_template_get_cache_id(const char* scenePath) {
    for (CreSceneCacheId cacheId = 0; cacheId < numberOfCachedSceneTemplates; cacheId++) {
        if (strcmp(cachedSceneTemplates[cacheId].path, scenePath) == 0) {
            return cacheId;
        }
    }
    return CRE_SCENE_CACHE_INVALID_ID;
}
