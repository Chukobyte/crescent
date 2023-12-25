#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <seika/math/se_math.h>

#include "../ecs/entity/entity.h"
#include "scene_manager.h"

struct SKATransformModel2D;

typedef EntityArray (*on_get_self_and_parent_entities) (CreEntity);
typedef SKATransform2D (*on_get_local_transform) (CreEntity, int*, bool*);

void cre_scene_utils_update_global_transform_model(CreEntity entity, SKATransformModel2D* globalTransform);
void cre_scene_utils_apply_camera_and_origin_translation(SKATransformModel2D* globalTransform, SKAVector2* offset, bool ignoreCamera);
void cre_scene_utils_override_on_get_self_and_parent_entities_func(on_get_self_and_parent_entities func);
void cre_scene_utils_override_on_get_local_transform_func(on_get_local_transform func);
EntityArray cre_scene_utils_get_self_and_parent_entities(CreEntity entity);
void cre_scene_utils_reset_callback_func_overrides();

#ifdef __cplusplus
}
#endif
