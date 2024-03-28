#include "transform2d_component.h"

#include <string.h>

#include <seika/logger.h>
#include <seika/memory.h>

Transform2DComponent* transform2d_component_create() {
    Transform2DComponent* transform2DComponent = SKA_MEM_ALLOCATE(Transform2DComponent);
    transform2DComponent->localTransform = SKA_TRANSFORM_IDENTITY;
    transform2DComponent->globalTransform = SKA_TRANSFORM_MODEL_IDENTITY;
    transform2DComponent->zIndex = 0;
    transform2DComponent->isZIndexRelativeToParent = true;
    transform2DComponent->ignoreCamera = false;
    transform2DComponent->isGlobalTransformDirty = true; // Starts dirty to calculate on first pull
    return transform2DComponent;
}

void transform2d_component_delete(Transform2DComponent* transform2DComponent) {
    SKA_MEM_FREE(transform2DComponent);
}

Transform2DComponent* transform2d_component_copy(const Transform2DComponent* transform2DComponent) {
    Transform2DComponent* copiedNode = SKA_MEM_ALLOCATE(Transform2DComponent);
    memcpy(copiedNode, transform2DComponent, sizeof(Transform2DComponent));
    return copiedNode;
}

void transform2d_component_get_local_model_matrix(mat4 model, const SkaTransform2D* transform) {
    glm_mat4_identity(model);
    // 1. Translation
    glm_translate(model, (vec3) {
        transform->position.x, transform->position.y, 0.0f
    });
    // 2. Rotation
    f32 rotation = transform->rotation;
    glm_make_rad(&rotation);
    glm_rotate(model, rotation, (vec3) {
        0.0f, 0.0f, 1.0f
    });
    // 3. Scaling
    glm_scale(model, (vec3) {
        transform->scale.x, transform->scale.y, 1.0f
    });
}

f32 transform2d_component_get_rotation_deg_from_model(mat4 model) {
    versor quat;
    glm_mat4_quat(model, quat);
    const f32 angleRadians = glm_quat_angle(quat);
    return glm_deg(angleRadians);
}

void transform2d_component_print(Transform2DComponent* transform2DComponent) {
    ska_logger_message("{ position: (%f, %f), scale: (%f, %f), rotation: %f }",
                        transform2DComponent->localTransform.position.x,
                        transform2DComponent->localTransform.position.y,
                        transform2DComponent->localTransform.scale.x, transform2DComponent->localTransform.scale.y,
                        transform2DComponent->localTransform.rotation);
}
