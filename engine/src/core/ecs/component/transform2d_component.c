#include "transform2d_component.h"

#include "../../utils/logger.h"
#include "../../memory/rbe_mem.h"

Transform2D transform2d_component_create_blank_transform2d() {
    Transform2D transform2D = {
        .position = { .x = 0.0f, .y = 0.0f },
        .scale = { .x = 1.0f, .y = 1.0f },
        .rotation = 0.0f
    };
    return transform2D;
}

TransformModel2D transform2d_component_create_blank_global_transform2d() {
    TransformModel2D transform2D = {
        .position = { .x = 0.0f, .y = 0.0f },
        .scale = { .x = 1.0f, .y = 1.0f },
        .rotation = 0.0f,
        .scaleSign = { .x = 1.0f, .y = 1.0f }
    };
    return transform2D;
}

Transform2DComponent* transform2d_component_create() {
    Transform2DComponent* transform2DComponent = RBE_MEM_ALLOCATE(Transform2DComponent);
    transform2DComponent->localTransform = transform2d_component_create_blank_transform2d();
    transform2DComponent->globalTransform = transform2d_component_create_blank_global_transform2d();
    transform2DComponent->zIndex = 0;
    transform2DComponent->isZIndexRelativeToParent = true;
    transform2DComponent->ignoreCamera = false;
    transform2DComponent->isGlobalTransformDirty = true; // Starts dirty to calculate on first pull
    return transform2DComponent;
}

void transform2d_component_get_local_model_matrix(mat4 model, Transform2DComponent* transform2DComponent) {
    glm_mat4_identity(model);
    // 1. Translation
    glm_translate(model, (vec3) {
        transform2DComponent->localTransform.position.x, transform2DComponent->localTransform.position.y, 0.0f
    });
    // 2. Rotation
    float rotation = transform2DComponent->localTransform.rotation;
    glm_make_rad(&rotation);
    glm_rotate(model, rotation, (vec3) {
        0.0f, 0.0f, 1.0f
    });
    // 3. Scaling
    glm_scale(model, (vec3) {
        transform2DComponent->localTransform.scale.x, transform2DComponent->localTransform.scale.y, 1.0f
    });
}

//Vector2 transform2d_component_get_position_from_model(mat4 model) {
//    Vector2 position = { .x=model[3][0], .y=model[3][1] };
//    return position;
//}

//Vector2 transform2d_component_get_scale_from_model(mat4 model) {
//    Vector2 magnitude = {
//        .x = sqrt(model[0][0] * model[0][0] + model[0][1] * model[0][1] + model[0][2] * model[0][2] + model[0][3] * model[0][3]),
//        .y = sqrt(model[1][0] * model[1][0] + model[1][1] * model[1][1] + model[1][2] * model[1][2] + model[1][3] * model[1][3])
//    };
//    return magnitude;
//}

float transform2d_component_get_rotation_deg_from_model(mat4 model) {
    versor quat;
    glm_mat4_quat(model, quat);
    const float angleRadians = glm_quat_angle(quat);
    return glm_deg(angleRadians);
}

//Transform2DComponent transform2d_component_decompose_trs_matrix(mat4 model) {
//    Vector2 position = transform2d_component_get_position_from_model(model);
//    Vector2 scale = transform2d_component_get_scale_from_model(model);
//
//    Transform2DComponent transform2DComponent = {
//        .localTransform.position = position
//    };
//    return transform2DComponent;
//}

void transform2d_component_print(Transform2DComponent* transform2DComponent) {
    rbe_logger_print_err("{ position: (%f, %f), scale: (%f, %f), rotation: %f }",
                         transform2DComponent->localTransform.position.x, transform2DComponent->localTransform.position.y,
                         transform2DComponent->localTransform.scale.x, transform2DComponent->localTransform.scale.y,
                         transform2DComponent->localTransform.rotation);
}

