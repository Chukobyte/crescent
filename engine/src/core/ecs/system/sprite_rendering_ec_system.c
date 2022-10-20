#include "sprite_rendering_ec_system.h"

#include <string.h>

#include "../seika/src/rendering/renderer.h"
#include "../seika/src/utils/se_string_util.h"
#include "../seika/src/utils/se_assert.h"

#include "ec_system.h"
#include "../component/transform2d_component.h"
#include "../component/sprite_component.h"
#include "../../scene/scene_manager.h"
#include "../../camera/camera.h"
#include "../../camera/camera_manager.h"
#include "../../scene/scene_utils.h"

EntitySystem* spriteRenderingSystem = NULL;

void sprite_rendering_system_render();

EntitySystem* sprite_rendering_ec_system_create() {
    SE_ASSERT(spriteRenderingSystem == NULL);
    spriteRenderingSystem = rbe_ec_system_create();
    spriteRenderingSystem->name = se_strdup("Sprite Rendering");
    spriteRenderingSystem->render_func = sprite_rendering_system_render;
    spriteRenderingSystem->component_signature = ComponentType_TRANSFORM_2D | ComponentType_SPRITE;
    return spriteRenderingSystem;
}

void sprite_rendering_system_render() {
    const RBECamera2D* camera2D = rbe_camera_manager_get_current_camera();
    const RBECamera2D* defaultCamera = rbe_camera_manager_get_default_camera();

    for (size_t i = 0; i < spriteRenderingSystem->entity_count; i++) {
        const Entity entity = spriteRenderingSystem->entities[i];
        Transform2DComponent* spriteTransformComp = (Transform2DComponent*) component_manager_get_component(entity, ComponentDataIndex_TRANSFORM_2D);
        SpriteComponent* spriteComponent = (SpriteComponent*) component_manager_get_component(entity, ComponentDataIndex_SPRITE);
        const RBECamera2D* renderCamera = spriteTransformComp->ignoreCamera ? defaultCamera : camera2D;
        TransformModel2D* globalTransform = rbe_scene_manager_get_scene_node_global_transform(entity, spriteTransformComp);
        cre_scene_utils_apply_camera_and_origin_translation(globalTransform, &spriteComponent->origin, spriteTransformComp->ignoreCamera);
        spriteTransformComp->isGlobalTransformDirty = true; // TODO: Make global transform const
        const Size2D destinationSize = {
            spriteComponent->drawSource.w * renderCamera->zoom.x,
            spriteComponent->drawSource.h * renderCamera->zoom.y
        };
        se_renderer_queue_sprite_draw_call(
            spriteComponent->texture,
            spriteComponent->drawSource,
            destinationSize,
            spriteComponent->modulate,
            spriteComponent->flipX,
            spriteComponent->flipY,
            globalTransform
        );
    }
}
