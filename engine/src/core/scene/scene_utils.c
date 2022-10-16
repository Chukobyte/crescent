#include "scene_utils.h"

#include "scene_tree.h"
#include "scene_manager.h"
#include "../ecs/component/component.h"
#include "../camera/camera.h"
#include "../camera/camera_manager.h"

EntityArray default_get_self_and_parent_nodes(Entity entity);
Transform2D default_get_local_transform(Entity entity, bool* success);

on_get_self_and_parent_entities onGetSelfAndParentEntitiesFunc = &default_get_self_and_parent_nodes;
on_get_local_transform onGetLocalTransformFunc = &default_get_local_transform;

// Default engine callbacks

// First index is the child
EntityArray default_get_self_and_parent_nodes(Entity entity) {
    EntityArray combineModelResult = { .entityCount = 0 };
    combineModelResult.entities[combineModelResult.entityCount++] = entity;

    SceneTreeNode* sceneTreeNode = rbe_scene_manager_get_entity_tree_node(entity);
    SceneTreeNode* parentTreeNode = sceneTreeNode->parent;
    while (parentTreeNode != NULL) {
        combineModelResult.entities[combineModelResult.entityCount++] = parentTreeNode->entity;
        parentTreeNode = parentTreeNode->parent;
    }
    return combineModelResult;
}

Transform2D default_get_local_transform(Entity entity, bool* success) {
    Transform2DComponent* transform2DComponent = component_manager_get_component_unsafe(entity, ComponentDataIndex_TRANSFORM_2D);
    if (transform2DComponent == NULL) {
        *success = false;
        return (Transform2D) {};
    }
    *success = true;
    return transform2DComponent->localTransform;
}

void cre_scene_utils_update_global_transform_model(Entity entity, TransformModel2D* globalTransform) {
    glm_mat4_identity(globalTransform->model);
    EntityArray combineModelResult = onGetSelfAndParentEntitiesFunc(entity);
    Vector2 scaleTotal = { 1.0f, 1.0f };
    for (int i = combineModelResult.entityCount - 1; i >= 0; i--) {
        Entity currentEntity = combineModelResult.entities[i];
        bool hasLocalTransform = false;
        const Transform2D localTransform = onGetLocalTransformFunc(currentEntity, &hasLocalTransform);
        if (!hasLocalTransform) {
            continue;
        }
        scaleTotal.x *= localTransform.scale.x;
        scaleTotal.y *= localTransform.scale.y;
        mat4 newModel;
        transform2d_component_get_local_model_matrix(newModel, &localTransform);
        glm_mat4_mul(globalTransform->model, newModel, globalTransform->model);
    }
    globalTransform->scaleSign = rbe_math_signvec2(&scaleTotal);
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

void cre_scene_utils_apply_camera_and_origin_translation(TransformModel2D* globalTransform, Vector2* origin, bool ignoreCamera) {
    const RBECamera2D* renderCamera = ignoreCamera ? rbe_camera_manager_get_default_camera() : rbe_camera_manager_get_current_camera();
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

void cre_scene_utils_reset_callback_func_overrides() {
    onGetSelfAndParentEntitiesFunc = &default_get_self_and_parent_nodes;
    onGetLocalTransformFunc = &default_get_local_transform;
}
