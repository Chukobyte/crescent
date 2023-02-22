#include "font_rendering_ec_system.h"

#include "../seika/src/rendering/renderer.h"
#include "../seika/src/utils/se_string_util.h"
#include "../seika/src/utils/se_assert.h"

#include "ec_system.h"
#include "../component/transform2d_component.h"
#include "../component/text_label_component.h"
#include "../../scene/scene_manager.h"
#include "../../camera/camera.h"
#include "../../camera/camera_manager.h"


EntitySystem* fontRenderingSystem = NULL;

void font_rendering_system_render();

EntitySystem* font_rendering_ec_system_create() {
    SE_ASSERT(fontRenderingSystem == NULL);
    fontRenderingSystem = cre_ec_system_create();
    fontRenderingSystem->name = se_strdup("Font Rendering");
    fontRenderingSystem->render_func = font_rendering_system_render;
    fontRenderingSystem->component_signature = ComponentType_TRANSFORM_2D | ComponentType_TEXT_LABEL;
    return fontRenderingSystem;
}

void font_rendering_system_render() {
    const CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    const CRECamera2D* defaultCamera = cre_camera_manager_get_default_camera();
    for (size_t i = 0; i < fontRenderingSystem->entity_count; i++) {
        const Entity entity = fontRenderingSystem->entities[i];
        Transform2DComponent* fontTransformComp = (Transform2DComponent*) component_manager_get_component(entity, ComponentDataIndex_TRANSFORM_2D);
        TextLabelComponent* textLabelComponent = (TextLabelComponent*) component_manager_get_component(entity, ComponentDataIndex_TEXT_LABEL);
        const CRECamera2D* renderCamera = fontTransformComp->ignoreCamera ? defaultCamera : camera2D;
        fontTransformComp->isGlobalTransformDirty = true;
        const SETransformModel2D* globalTransform = cre_scene_manager_get_scene_node_global_transform(entity, fontTransformComp);

        se_renderer_queue_font_draw_call(
            textLabelComponent->font,
            textLabelComponent->text,
            (globalTransform->position.x - renderCamera->viewport.x + renderCamera->offset.x) * renderCamera->zoom.x,
            (globalTransform->position.y - renderCamera->viewport.y + renderCamera->offset.y) * renderCamera->zoom.y,
            fontTransformComp->localTransform.scale.x * globalTransform->scale.x * renderCamera->zoom.x,
            textLabelComponent->color,
            globalTransform->zIndex
        );
    }
}
