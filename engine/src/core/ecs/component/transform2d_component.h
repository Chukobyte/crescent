#pragma once

#include <stdbool.h>

#define CGLM_DEFINE_PRINTS
#include <cglm/cglm.h>

#include "../../math/rbe_math.h"

typedef struct Transform2DComponent {
    Transform2D localTransform;
    TransformModel2D globalTransform;
    int zIndex;
    bool isZIndexRelativeToParent;
    bool ignoreCamera;
    bool isGlobalTransformDirty;
} Transform2DComponent;

Transform2DComponent* transform2d_component_create();
void transform2d_component_get_local_model_matrix(mat4 model, Transform2DComponent* transform2DComponent);
float transform2d_component_get_rotation_deg_from_model(mat4 model);
void transform2d_component_print(Transform2DComponent* transform2DComponent);
