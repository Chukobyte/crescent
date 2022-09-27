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
Rect2 colliderDrawSource = { .x=0.0f, .y=0.0f, .w=1.0f, .h=1.0f };

void collision_system_physics_update(float deltaTime);
void collision_system_render();

EntitySystem* collision_ec_system_create() {
    RBE_ASSERT(collisionSystem == NULL);
    collisionSystem = rbe_ec_system_create();
    collisionSystem->name = rbe_strdup("Collision");
    collisionSystem->component_signature = ComponentType_TRANSFORM_2D | ComponentType_COLLIDER_2D;

    collisionSystem->physics_process_func = collision_system_physics_update;

    RBEGameProperties* gameProps = rbe_game_props_get();
    RBE_ASSERT(rbe_game_props_get() != NULL);
    if (gameProps->areCollidersVisible) {
        collisionSystem->render_func = collision_system_render;
        collisionOutlineTexture = rbe_texture_create_solid_colored_texture(1, 1, 255);
        RBE_ASSERT(collisionOutlineTexture != NULL);
    }
    return collisionSystem;
}

EntitySystem* collision_ec_system_get() {
    return collisionSystem;
}

// TODO: Temp, figure out how we want to handle caching the global transform
void collision_system_physics_update(float deltaTime) {
    for (size_t i = 0; i < collisionSystem->entity_count; i++) {
        const Entity entity = collisionSystem->entities[i];
        Transform2DComponent* transformComp = (Transform2DComponent *) component_manager_get_component(entity, ComponentDataIndex_TRANSFORM_2D);
        transformComp->isGlobalTransformDirty = true;
    }
}

void collision_system_render() {
    const RBECamera2D* camera2D = rbe_camera_manager_get_current_camera();
    const RBECamera2D* defaultCamera = rbe_camera_manager_get_default_camera();
    for (size_t i = 0; i < collisionSystem->entity_count; i++) {
        const Entity entity = collisionSystem->entities[i];
        Transform2DComponent* transformComp = (Transform2DComponent*) component_manager_get_component(entity, ComponentDataIndex_TRANSFORM_2D);
        const Collider2DComponent* colliderComp = (Collider2DComponent*) component_manager_get_component(entity, ComponentDataIndex_COLLIDER_2D);
        const RBECamera2D* renderCamera = transformComp->ignoreCamera ? defaultCamera : camera2D;
        TransformModel2D* globalTransform = rbe_scene_manager_get_scene_node_global_transform(entity, transformComp);
        transformComp->isGlobalTransformDirty = true; // TODO: Make global transform const
        const Size2D colliderDrawSize = {
            colliderComp->extents.w * renderCamera->zoom.x,
            colliderComp->extents.h * renderCamera->zoom.y
        };
        glm_translate(globalTransform->model, (vec3) {
            (renderCamera->offset.x - (renderCamera->viewport.x * globalTransform->scaleSign.x)) * renderCamera->zoom.x,
            (renderCamera->offset.y - (renderCamera->viewport.y * globalTransform->scaleSign.y)) * renderCamera->zoom.y,
            0.0f
        });
        rbe_renderer_queue_sprite_draw_call(
            collisionOutlineTexture,
            colliderDrawSource,
            colliderDrawSize,
            colliderComp->color,
            false,
            false,
            globalTransform
        );
    }
}
