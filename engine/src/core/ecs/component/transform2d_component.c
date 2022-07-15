#include "transform2d_component.h"

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

// Tests
Vector2 transform2d_component_get_position_from_model(mat4 model) {
    Vector2 position = { .x=model[3][0], .y=model[3][1] };
    return position;
}
