#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../ecs/entity/entity.h"
#include "../seika/src/math/rbe_math.h"

struct TransformModel2D;

typedef struct EntityArray {
    int entityCount;
    Entity entities[10];
} EntityArray;

typedef EntityArray (*on_get_self_and_parent_entities) (Entity);
typedef Transform2D (*on_get_local_transform) (Entity, bool*);

void cre_scene_utils_update_global_transform_model(Entity entity, TransformModel2D* globalTransform);
void cre_scene_utils_apply_camera_and_origin_translation(TransformModel2D* globalTransform, Vector2* offset, bool ignoreCamera);
void cre_scene_utils_override_on_get_self_and_parent_entities_func(on_get_self_and_parent_entities func);
void cre_scene_utils_override_on_get_local_transform_func(on_get_local_transform func);
void cre_scene_utils_reset_callback_func_overrides();

#ifdef __cplusplus
}
#endif
