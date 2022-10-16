#include "color_rect_ec_system.h"

#include <string.h>

#include "ec_system.h"
#include "../component/transform2d_component.h"
#include "../component/color_rect_component.h"
#include "../../rendering/renderer.h"
#include "../../scene/scene_manager.h"
#include "../../camera/camera.h"
#include "../../camera/camera_manager.h"
#include "../../scene/scene_utils.h"
#include "../../utils/rbe_string_util.h"
#include "../../utils/rbe_assert.h"

EntitySystem* colorRectSystem = NULL;
Texture* colorRectTexture = NULL;
Rect2 colorRectDrawSource = {0.0f, 0.0f, 1.0f, 1.0f };

void color_rect_system_render();

EntitySystem* color_rect_ec_system_create() {
    RBE_ASSERT(colorRectSystem == NULL);
    colorRectSystem = rbe_ec_system_create();
    colorRectSystem->name = rbe_strdup("Color Square");
    colorRectSystem->render_func = color_rect_system_render;
    colorRectSystem->component_signature = ComponentType_TRANSFORM_2D | ComponentType_COLOR_RECT;

    RBE_ASSERT_FMT(colorRectTexture == NULL, "Color Square Texture isn't NULL!");
    colorRectTexture = rbe_texture_create_solid_colored_texture(1, 1, 255);
    return colorRectSystem;
}

void color_rect_system_render() {
    const RBECamera2D* camera2D = rbe_camera_manager_get_current_camera();
    const RBECamera2D* defaultCamera = rbe_camera_manager_get_default_camera();

    for (size_t i = 0; i < colorRectSystem->entity_count; i++) {
        const Entity entity = colorRectSystem->entities[i];
        Transform2DComponent* transformComp = (Transform2DComponent*) component_manager_get_component(entity, ComponentDataIndex_TRANSFORM_2D);
        const ColorRectComponent* colorRectComponent = (ColorRectComponent *) component_manager_get_component(entity, ComponentDataIndex_COLOR_RECT);
        const RBECamera2D* renderCamera = transformComp->ignoreCamera ? defaultCamera : camera2D;
        TransformModel2D* globalTransform = rbe_scene_manager_get_scene_node_global_transform(entity, transformComp);
        static Vector2 origin = { 0.0f, 0.0f };
        cre_scene_utils_apply_camera_and_origin_translation(globalTransform, &origin, transformComp->ignoreCamera);
        transformComp->isGlobalTransformDirty = true; // TODO: Make global transform const
        const Size2D destinationSize = {
            colorRectComponent->size.w * renderCamera->zoom.x,
            colorRectComponent->size.h * renderCamera->zoom.y
        };
        rbe_renderer_queue_sprite_draw_call(
            colorRectTexture,
            colorRectDrawSource,
            destinationSize,
            colorRectComponent->color,
            false,
            false,
            globalTransform
        );
    }
}
