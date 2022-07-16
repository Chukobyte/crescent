#include "transform2d_component.h"

#include "../../utils/logger.h"
#include "../../memory/rbe_mem.h"

Transform2DComponent* transform2d_component_create() {
    Transform2DComponent* transform2DComponent = RBE_MEM_ALLOCATE(Transform2DComponent);
    transform2DComponent->position.x = 0.0f;
    transform2DComponent->position.y = 0.0f;
    transform2DComponent->scale.x = 1.0f;
    transform2DComponent->scale.y = 1.0f;
    transform2DComponent->rotation = 0.0f;
    transform2DComponent->zIndex = 0;
    transform2DComponent->isZIndexRelativeToParent = true;
    transform2DComponent->ignoreCamera = false;
    return transform2DComponent;
}

void transform2d_component_get_local_model_matrix(mat4 model, Transform2DComponent* transform2DComponent) {
    glm_mat4_identity(model);
    // 1. Translation
    glm_translate(model, (vec3) {
        transform2DComponent->position.x, transform2DComponent->position.y, 0.0f
    });
    // 2. Rotation
    float rotation = transform2DComponent->rotation;
    glm_make_rad(&rotation);
    glm_rotate(model, rotation, (vec3) {
        0.0f, 0.0f, 1.0f
    });
    // 3. Scaling
    glm_scale(model, (vec3) {
        transform2DComponent->scale.x, transform2DComponent->scale.y, 1.0f
    });
}

Vector2 transform2d_component_get_position_from_model(mat4 model) {
    Vector2 position = { .x=model[3][0], .y=model[3][1] };
    return position;
}

Vector2 transform2d_component_get_scale_from_model(mat4 model) {
    Vector2 magnitude = {
        .x = sqrt(model[0][0] * model[0][0] + model[0][1] * model[0][1] + model[0][2] * model[0][2] + model[0][3] * model[0][3]),
        .y = sqrt(model[1][0] * model[1][0] + model[1][1] * model[1][1] + model[1][2] * model[1][2] + model[1][3] * model[1][3])
    };
    return magnitude;
}

float transform2d_component_get_rotation_deg_from_model(mat4 model) {
    versor quat;
    glm_mat4_quat(model, quat);
    const float angleRadians = glm_quat_angle(quat);
    return glm_deg(angleRadians);
//    return angleRadians;

//    const float axisX = RBE_RAD_2_DEG * atan2f(model[1][2], model[2][2]);
//    const float axisY = RBE_RAD_2_DEG * atan2f(-model[0][2], sqrtf(model[1][2] * model[1][2] + model[2][2] * model[2][2]));
//    return (atan2f(axisY, axisX) / RBE_PI) * 180.0f;
}

Transform2DComponent transform2d_component_decompose_trs_matrix(mat4 model) {
    Vector2 position = transform2d_component_get_position_from_model(model);
    Vector2 scale = transform2d_component_get_scale_from_model(model);

    Transform2DComponent transform2DComponent = {
        .position = position
    };
    return transform2DComponent;
}

void transform2d_component_print(Transform2DComponent* transform2DComponent) {
    rbe_logger_print_err("{ position: (%f, %f), scale: (%f, %f), rotation: %f }",
                         transform2DComponent->position.x, transform2DComponent->position.y,
                         transform2DComponent->scale.x, transform2DComponent->scale.y, transform2DComponent->rotation);
}

