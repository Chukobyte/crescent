#include "scene_utils.h"

#include <seika/ecs/ecs.h>

#include "scene_tree.h"
#include "../ecs/ecs_globals.h"
#include "../ecs/component/component.h"
#include "../camera/camera.h"
#include "../camera/camera_manager.h"

SKATransform2D default_get_local_transform(SkaEntity entity, int* zIndex, bool* success);

on_get_self_and_parent_entities onGetSelfAndParentEntitiesFunc = &cre_scene_manager_get_self_and_parent_nodes;
on_get_local_transform onGetLocalTransformFunc = &default_get_local_transform;

// Default engine callbacks
SKATransform2D default_get_local_transform(SkaEntity entity, int* zIndex, bool* success) {
    Transform2DComponent* transform2DComponent = ska_ecs_component_manager_get_component_unchecked(entity, TRANSFORM2D_COMPONENT_INDEX);
    if (transform2DComponent == NULL) {
        *success = false;
        return SKA_TRANSFORM_IDENTITY;
    }
    *zIndex = transform2DComponent->zIndex;
    *success = true;
    return transform2DComponent->localTransform;
}

void cre_scene_utils_update_global_transform_model(SkaEntity entity, SKATransformModel2D* globalTransform) {
    glm_mat4_identity(globalTransform->model);
    EntityArray combineModelResult = onGetSelfAndParentEntitiesFunc(entity);
    SKAVector2 scaleTotal = SKA_VECTOR2_ONE;
    globalTransform->zIndex = 0;
    for (int i = combineModelResult.entityCount - 1; i >= 0; i--) {
        SkaEntity currentEntity = combineModelResult.entities[i];
        bool hasLocalTransform = false;
        int localZIndex = 0;
        const SKATransform2D localTransform = onGetLocalTransformFunc(currentEntity, &localZIndex, &hasLocalTransform);
        if (!hasLocalTransform) {
            continue;
        }
        globalTransform->zIndex += localZIndex;
        scaleTotal.x *= localTransform.scale.x;
        scaleTotal.y *= localTransform.scale.y;
        mat4 newModel;
        transform2d_component_get_local_model_matrix(newModel, &localTransform);
        glm_mat4_mul(globalTransform->model, newModel, globalTransform->model);
    }
    globalTransform->scaleSign = ska_math_signvec2(&scaleTotal);
    // Decompose trs matrix
    vec4 translation;
    mat4 rotation;
    vec3 scale;
    glm_decompose(globalTransform->model, translation, rotation, scale);
    // Update global transform
    globalTransform->position.x = translation[0];
    globalTransform->position.y = translation[1];
    // Scale sign is used to fix sign of scale not being properly decomposed in trs matrix
    globalTransform->scale.x = fabsf(scale[0]) * globalTransform->scaleSign.x;
    globalTransform->scale.y = fabsf(scale[1]) * globalTransform->scaleSign.y;
    globalTransform->rotation = transform2d_component_get_rotation_deg_from_model(rotation);
}

void cre_scene_utils_apply_camera_and_origin_translation(SKATransformModel2D* globalTransform, const SKAVector2* origin, bool ignoreCamera) {
    const CRECamera2D* renderCamera = ignoreCamera ? cre_camera_manager_get_default_camera() : cre_camera_manager_get_current_camera();
    glm_translate(globalTransform->model, (vec3) {
        (renderCamera->offset.x - (renderCamera->viewport.x * globalTransform->scaleSign.x) - origin->x) * renderCamera->zoom.x,
        (renderCamera->offset.y - (renderCamera->viewport.y * globalTransform->scaleSign.y) - origin->y) * renderCamera->zoom.y,
        0.0f
    });
}

void cre_scene_utils_override_on_get_self_and_parent_entities_func(on_get_self_and_parent_entities func) {
    onGetSelfAndParentEntitiesFunc = func;
}

void cre_scene_utils_override_on_get_local_transform_func(on_get_local_transform func) {
    onGetLocalTransformFunc = func;
}

EntityArray cre_scene_utils_get_self_and_parent_entities(SkaEntity entity) {
    return onGetSelfAndParentEntitiesFunc(entity);
}

void cre_scene_utils_reset_callback_func_overrides() {
    onGetSelfAndParentEntitiesFunc = &cre_scene_manager_get_self_and_parent_nodes;
    onGetLocalTransformFunc = &default_get_local_transform;
}
