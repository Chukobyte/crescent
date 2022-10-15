#pragma once

#include "../ecs/entity/entity.h"

struct TransformModel2D;

void cre_scene_utils_update_global_transform_model(Entity entity, struct TransformModel2D* globalTransform);
