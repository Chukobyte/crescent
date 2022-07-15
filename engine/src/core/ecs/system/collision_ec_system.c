#include "collision_ec_system.h"

#include "ec_system.h"
#include "../component/transform2d_component.h"
#include "../component/collider2d_component.h"
#include "../../scene/scene_manager.h"
#include "../../game_properties.h"
#include "../../rendering/renderer.h"
#include "../../camera/camera.h"
#include "../../camera/camera_manager.h"
#include "../../utils/rbe_string_util.h"
#include "../../utils/rbe_assert.h"

EntitySystem* collisionSystem = NULL;
Texture* collisionOutlineTexture = NULL;
Color collisionDrawColor = { .r=1.0f, .g=1.0f, .b=1.0f, .a=1.0f };
Rect2 colliderDrawSource = { .x=0.0f, .y=0.0f, .w=1.0f, .h=1.0f };

void collision_system_render();

EntitySystem* collision_ec_system_create() {
    RBE_ASSERT(collisionSystem == NULL);
    collisionSystem = rbe_ec_system_create();
    collisionSystem->name = rbe_strdup("Collision");
    collisionSystem->component_signature = ComponentType_TRANSFORM_2D | ComponentType_COLLIDER_2D;

    RBEGameProperties* gameProps = rbe_game_props_get();
    RBE_ASSERT(rbe_game_props_get() != NULL);
    if (gameProps->areCollidersVisible) {
        collisionSystem->render_func = collision_system_render;
        collisionOutlineTexture = rbe_texture_create_solid_colored_texture(1, 1, 255);
        RBE_ASSERT(collisionOutlineTexture);
        collisionDrawColor.r = 1.0f;
        collisionDrawColor.g = 1.0f;
        collisionDrawColor.b = 1.0f;
        collisionDrawColor.a = 1.0f;
    }
    return collisionSystem;
}

EntitySystem* collision_ec_system_get() {
    return collisionSystem;
}

void collision_system_render() {
    const RBECamera2D* camera2D = rbe_camera_manager_get_current_camera();
    const RBECamera2D* defaultCamera = rbe_camera_manager_get_default_camera();
    for (size_t i = 0; i < collisionSystem->entity_count; i++) {
        const Entity entity = collisionSystem->entities[i];
        const Transform2DComponent* transformComp = (Transform2DComponent*) component_manager_get_component(entity, ComponentDataIndex_TRANSFORM_2D);
        const Collider2DComponent* colliderComp = (Collider2DComponent*) component_manager_get_component(entity, ComponentDataIndex_COLLIDER_2D);
        const RBECamera2D* renderCamera = transformComp->ignoreCamera ? defaultCamera : camera2D;
        const Transform2DComponent combinedTransform = rbe_scene_manager_get_scene_graph_transform(entity);
        const Rect2 colliderDrawDestination = {
            (combinedTransform.position.x - renderCamera->viewport.x + renderCamera->offset.x) * renderCamera->zoom.x,
            (combinedTransform.position.y - renderCamera->viewport.y + renderCamera->offset.y) * renderCamera->zoom.y,
            combinedTransform.scale.x * colliderComp->rect.w * renderCamera->zoom.x,
            combinedTransform.scale.y * colliderComp->rect.h * renderCamera->zoom.y
        };

        rbe_renderer_queue_sprite_draw_call(
            collisionOutlineTexture,
            colliderDrawSource,
            colliderDrawDestination,
            transformComp->rotation,
            colliderComp->color,
            false,
            false
        );
    }
}
