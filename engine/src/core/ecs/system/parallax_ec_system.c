#include "parallax_ec_system.h"

#include "../seika/src/utils/se_assert.h"
#include "../seika/src/utils/se_string_util.h"

#include "ec_system.h"
#include "../component/transform2d_component.h"
#include "../component/parallax_component.h"
#include "../../camera/camera.h"
#include "../../camera/camera_manager.h"

EntitySystem* parallaxSystem = NULL;

void parallax_system_physics_process(float deltaTime);

struct EntitySystem* parallax_ec_system_create() {
    SE_ASSERT(parallaxSystem == NULL);
    parallaxSystem = cre_ec_system_create();
    parallaxSystem->name = se_strdup("Parallax");
    parallaxSystem->physics_process_func = parallax_system_physics_process;
    parallaxSystem->component_signature = ComponentType_TRANSFORM_2D | ComponentType_PARALLAX;
    return parallaxSystem;
}

void parallax_system_update_entity(Entity entity, Transform2DComponent* transformComp, ParallaxComponent* parallaxComp, CRECamera2D* camera2D) {
    // Save off cache data
    parallaxComp->cachedScrollSpeed = parallaxComp->scrollSpeed;
    parallaxComp->cachedLocalPosition = transformComp->localTransform.position;
    // Calculate offset and apply to entity's local transform
    const Vector2 offset = {
        .x = (transformComp->localTransform.position.x - (camera2D->viewport.x + camera2D->offset.x)) * camera2D->zoom.x * parallaxComp->scrollSpeed.x,
        .y = (transformComp->localTransform.position.y - (camera2D->viewport.y + camera2D->offset.y)) * camera2D->zoom.y * parallaxComp->scrollSpeed.y
    };
    transformComp->localTransform.position.x += offset.x;
    transformComp->localTransform.position.y += offset.y;
    // TODO: Delete once it is working...
//    positionOffset.x = (transform->position.x - (camera->viewport.x + camera->cachedScrollSpeed.x) + owner->parentOffset.x) * camera->zoom * scrollSpeed.x;
//    positionOffset.y = (transform->position.y - (camera->viewport.y + camera->cachedScrollSpeed.y) + owner->parentOffset.y) * camera->zoom * scrollSpeed.y;
}

void parallax_system_physics_process(float deltaTime) {
    CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    for (size_t i = 0; i < parallaxSystem->entity_count; i++) {
        const Entity entity = parallaxSystem->entities[i];
        Transform2DComponent* transformComp = (Transform2DComponent*) component_manager_get_component(entity, ComponentDataIndex_TRANSFORM_2D);
        ParallaxComponent* parallaxComp = (ParallaxComponent*) component_manager_get_component(entity, ComponentDataIndex_PARALLAX);
        // Only update if local position or scroll offsets change
        if (!se_math_vec2_equals(&parallaxComp->cachedLocalPosition, &transformComp->localTransform.position) || !se_math_vec2_equals(&parallaxComp->cachedScrollSpeed, &parallaxComp->scrollSpeed)) {
            parallax_system_update_entity(entity, transformComp, parallaxComp, camera2D);
        }
    }
}
