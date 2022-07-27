#include "color_square_ec_system.h"

#include <string.h>

#include "ec_system.h"
#include "../component/transform2d_component.h"
#include "../component/color_square_component.h"
#include "../../rendering/renderer.h"
#include "../../scene/scene_manager.h"
#include "../../camera/camera.h"
#include "../../camera/camera_manager.h"
#include "../../utils/rbe_string_util.h"
#include "../../utils/rbe_assert.h"

EntitySystem* colorSquareSystem = NULL;
Texture* colorSquareTexture = NULL;
Rect2 colorSquareDrawSource = { 0.0f, 0.0f, 1.0f, 1.0f };

void color_square_system_render();

EntitySystem* color_square_ec_system_create() {
    RBE_ASSERT(colorSquareSystem == NULL);
    colorSquareSystem = rbe_ec_system_create();
    colorSquareSystem->name = rbe_strdup("Color Square");
    colorSquareSystem->render_func = color_square_system_render;
    colorSquareSystem->component_signature = ComponentType_TRANSFORM_2D | ComponentType_COLOR_SQUARE;

    RBE_ASSERT_FMT(colorSquareTexture == NULL, "Color Square Texture isn't NULL!");
    colorSquareTexture = rbe_texture_create_solid_colored_texture(1, 1, 255);
    return colorSquareSystem;
}

void color_square_system_render() {
    const RBECamera2D* camera2D = rbe_camera_manager_get_current_camera();
    const RBECamera2D* defaultCamera = rbe_camera_manager_get_default_camera();

    for (size_t i = 0; i < colorSquareSystem->entity_count; i++) {
        const Entity entity = colorSquareSystem->entities[i];
        Transform2DComponent* transformComp = (Transform2DComponent*) component_manager_get_component(entity, ComponentDataIndex_TRANSFORM_2D);
        const ColorSquareComponent* colorSquareComponent = (ColorSquareComponent *) component_manager_get_component(entity, ComponentDataIndex_COLOR_SQUARE);
        const RBECamera2D* renderCamera = transformComp->ignoreCamera ? defaultCamera : camera2D;
        TransformModel2D* globalTransform = rbe_scene_manager_get_scene_node_global_transform(entity, transformComp);
        transformComp->isGlobalTransformDirty = true; // TODO: Make global transform const
        const Size2D destinationSize = {
            colorSquareComponent->size.w * renderCamera->zoom.x,
            colorSquareComponent->size.h * renderCamera->zoom.y
        };

        glm_translate(globalTransform->model, (vec3) {
            (renderCamera->offset.x - (renderCamera->viewport.x * globalTransform->scaleSign.x)) * renderCamera->zoom.x,
            (renderCamera->offset.y - (renderCamera->viewport.y * globalTransform->scaleSign.y)) * renderCamera->zoom.y,
            0.0f
        });

        rbe_renderer_queue_sprite_draw_call(
            colorSquareTexture,
            colorSquareDrawSource,
            destinationSize,
            colorSquareComponent->color,
            false,
            false,
            globalTransform
        );
    }
}
