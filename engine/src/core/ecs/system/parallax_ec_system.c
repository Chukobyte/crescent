#include "parallax_ec_system.h"

#include "../seika/src/utils/se_assert.h"
#include "../seika/src/utils/se_string_util.h"

#include "ec_system.h"
#include "../component/transform2d_component.h"
#include "../component/parallax_component.h"
#include "../../camera/camera.h"
#include "../../camera/camera_manager.h"

EntitySystem* parallaxSystem = NULL;

void parallax_system_on_entity_entered_scene(Entity entity);
void parallax_system_physics_process(float deltaTime);

void parallax_system_update_entity(Entity entity, Transform2DComponent* transformComp, ParallaxComponent* parallaxComp, CRECamera2D* camera2D);

struct EntitySystem* parallax_ec_system_create() {
    SE_ASSERT(parallaxSystem == NULL);
    parallaxSystem = cre_ec_system_create();
    parallaxSystem->name = se_strdup("Parallax");
    parallaxSystem->on_entity_entered_scene_func = parallax_system_on_entity_entered_scene;
    parallaxSystem->physics_process_func = parallax_system_physics_process;
    parallaxSystem->component_signature = ComponentType_TRANSFORM_2D | ComponentType_PARALLAX;
    return parallaxSystem;
}

void parallax_system_on_entity_entered_scene(Entity entity) {
    Transform2DComponent* transformComp = (Transform2DComponent*) component_manager_get_component_unsafe(entity, ComponentDataIndex_TRANSFORM_2D);
    ParallaxComponent* parallaxComp = (ParallaxComponent*) component_manager_get_component_unsafe(entity, ComponentDataIndex_PARALLAX);
    SE_ASSERT(transformComp != NULL);
    SE_ASSERT(parallaxComp != NULL);
    parallaxComp->cachedScrollSpeed = parallaxComp->scrollSpeed;
    parallaxComp->cachedLocalPosition = transformComp->localTransform.position;
    parallax_system_update_entity(entity, transformComp, parallaxComp, cre_camera_manager_get_current_camera());
}

void parallax_system_physics_process(float deltaTime) {
    CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    for (size_t i = 0; i < parallaxSystem->entity_count; i++) {
        const Entity entity = parallaxSystem->entities[i];
        Transform2DComponent* transformComp = (Transform2DComponent*) component_manager_get_component(entity, ComponentDataIndex_TRANSFORM_2D);
        ParallaxComponent* parallaxComp = (ParallaxComponent*) component_manager_get_component(entity, ComponentDataIndex_PARALLAX);
        parallax_system_update_entity(entity, transformComp, parallaxComp, camera2D);
    }
}

void parallax_system_update_entity(Entity entity, Transform2DComponent* transformComp, ParallaxComponent* parallaxComp, CRECamera2D* camera2D) {
    // TODO: Check for external move changes...
    const Vector2 offset = {
        .x = (transformComp->localTransform.position.x - (camera2D->viewport.x + camera2D->offset.x)) * camera2D->zoom.x * parallaxComp->scrollSpeed.x,
        .y = (transformComp->localTransform.position.y - (camera2D->viewport.y + camera2D->offset.y)) * camera2D->zoom.y * parallaxComp->scrollSpeed.y
    };
    transformComp->localTransform.position.x = parallaxComp->cachedLocalPosition.x + offset.x;
    transformComp->localTransform.position.y = parallaxComp->cachedLocalPosition.y + offset.y;
}
