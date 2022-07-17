#include "font_rendering_ec_system.h"

#include "ec_system.h"
#include "../component/transform2d_component.h"
#include "../component/text_label_component.h"
#include "../../rendering/renderer.h"
#include "../../scene/scene_manager.h"
#include "../../camera/camera.h"
#include "../../camera/camera_manager.h"
#include "../../utils/rbe_string_util.h"
#include "../../utils/rbe_assert.h"


EntitySystem* fontRenderingSystem = NULL;

void font_rendering_system_render();

EntitySystem* font_rendering_ec_system_create() {
    RBE_ASSERT(fontRenderingSystem == NULL);
    fontRenderingSystem = rbe_ec_system_create();
    fontRenderingSystem->name = rbe_strdup("Font Rendering");
    fontRenderingSystem->render_func = font_rendering_system_render;
    fontRenderingSystem->component_signature = ComponentType_TRANSFORM_2D | ComponentType_TEXT_LABEL;
    return fontRenderingSystem;
}

void font_rendering_system_render() {
    const RBECamera2D* camera2D = rbe_camera_manager_get_current_camera();
    const RBECamera2D* defaultCamera = rbe_camera_manager_get_default_camera();
    for (size_t i = 0; i < fontRenderingSystem->entity_count; i++) {
        const Entity entity = fontRenderingSystem->entities[i];
        Transform2DComponent* fontTransformComp = (Transform2DComponent*) component_manager_get_component(entity, ComponentDataIndex_TRANSFORM_2D);
        TextLabelComponent* textLabelComponent = (TextLabelComponent*) component_manager_get_component(entity, ComponentDataIndex_TEXT_LABEL);
        const RBECamera2D* renderCamera = fontTransformComp->ignoreCamera ? defaultCamera : camera2D;
        const TransformModel2D* globalTransform = rbe_scene_manager_get_scene_graph_transform(entity);

        rbe_renderer_queue_font_draw_call(
            textLabelComponent->font,
            textLabelComponent->text,
            (globalTransform->position.x - renderCamera->viewport.x + renderCamera->offset.x) * renderCamera->zoom.x,
            (globalTransform->position.y - renderCamera->viewport.y + renderCamera->offset.y) * renderCamera->zoom.y,
            fontTransformComp->localTransform.scale.x * globalTransform->scale.x * renderCamera->zoom.x,
            textLabelComponent->color
        );
    }
}
