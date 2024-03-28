#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#include <seika/rendering/shader/shader_instance_minimal.h>
#include <seika/math/curve_float.h>
#include <seika/data_structures/array_list.h>

#include "../ecs/component.h"
#include "../ecs/components/node_component.h"

typedef struct JsonSceneNode {
    char* name;
    NodeBaseType type;
    char* externalNodeSource;
    bool fromExternalNodeSource;
    SkaArrayList* tags;
    void* components[CRE_MAX_COMPONENTS];
    struct JsonSceneNode* parent;
    struct JsonSceneNode* children[25]; // TODO: Change to an array list
    size_t childrenCount;
    char* spriteTexturePath;
    char* fontUID;
    char* shaderInstanceShaderPath;
    char* shaderInstanceVertexPath;
    char* shaderInstanceFragmentPath;
} JsonSceneNode;

struct CREGameProperties* cre_json_load_config_file(const char* filePath);
JsonSceneNode* cre_json_load_scene_file(const char* filePath);
void cre_json_delete_json_scene_node(JsonSceneNode* node);
SkaCurveFloat cre_json_load_curve_float_file(const char* filePath, bool *isSuccessful);

#ifdef __cplusplus
}
#endif
