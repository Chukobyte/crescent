#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include <seika/utils/observer.h>
#include <seika/math/se_math.h>

typedef struct Transform2DComponent {
    SKATransform2D localTransform;
    SKATransformModel2D globalTransform;
    int zIndex;
    bool isZIndexRelativeToParent;
    bool ignoreCamera;
    bool isGlobalTransformDirty;
    SEEvent onTransformChanged; // { data = ComponentEntityUpdatePayload(self), type = 0 (not used) }
} Transform2DComponent;

Transform2DComponent* transform2d_component_create();
void transform2d_component_delete(Transform2DComponent* transform2DComponent);
Transform2DComponent* transform2d_component_copy(const Transform2DComponent* transform2DComponent);
void transform2d_component_get_local_model_matrix(mat4 model, const SKATransform2D* transform);
float transform2d_component_get_rotation_deg_from_model(mat4 model);
void transform2d_component_print(Transform2DComponent* transform2DComponent);

#ifdef __cplusplus
}
#endif
