#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <seika/math/math.h>

#include "scene_manager.h"

struct SkaTransformModel2D;

typedef EntityArray (*on_get_self_and_parent_entities) (SkaEntity);
typedef SkaTransform2D (*on_get_local_transform) (SkaEntity, int*, bool*);

void cre_scene_utils_update_global_transform_model(SkaEntity entity, SkaTransformModel2D* globalTransform);
void cre_scene_utils_apply_camera_and_origin_translation(SkaTransformModel2D* globalTransform, const SkaVector2* offset, bool ignoreCamera);
void cre_scene_utils_override_on_get_self_and_parent_entities_func(on_get_self_and_parent_entities func);
void cre_scene_utils_override_on_get_local_transform_func(on_get_local_transform func);
EntityArray cre_scene_utils_get_self_and_parent_entities(SkaEntity entity);
void cre_scene_utils_reset_callback_func_overrides();

#ifdef __cplusplus
}
#endif
