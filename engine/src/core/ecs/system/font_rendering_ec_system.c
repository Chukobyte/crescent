#include "font_rendering_ec_system.h"

#include "../../rendering/renderer.h"
#include "../../scene/scene_manager.h"
#include "../../utils/rbe_assert.h"
#include "../../utils/rbe_string_util.h"
#include "../component/text_label_component.h"
#include "../component/transform2d_component.h"
#include "ec_system.h"

EntitySystem* fontRenderingSystem = NULL;

void font_rendering_system_render();

EntitySystem* font_rendering_ec_system_create() {
    RBE_ASSERT(fontRenderingSystem == NULL);
    fontRenderingSystem                      = rbe_ec_system_create();
    fontRenderingSystem->name                = rbe_strdup("Font Rendering");
    fontRenderingSystem->render_func         = font_rendering_system_render;
    fontRenderingSystem->component_signature = ComponentType_TRANSFORM_2D | ComponentType_TEXT_LABEL;
    return fontRenderingSystem;
}

void font_rendering_system_render() {
    for (size_t i = 0; i < fontRenderingSystem->entity_count; i++) {
        const Entity entity                        = fontRenderingSystem->entities[i];
        const Transform2DComponent parentTransform = rbe_scene_manager_get_combined_parent_transform(entity);
        Transform2DComponent* fontTransformComp =
            (Transform2DComponent*) component_manager_get_component(entity, ComponentDataIndex_TRANSFORM_2D);
        TextLabelComponent* textLabelComponent =
            (TextLabelComponent*) component_manager_get_component(entity, ComponentDataIndex_TEXT_LABEL);
        rbe_renderer_queue_font_draw_call(textLabelComponent->font, textLabelComponent->text,
                                          fontTransformComp->position.x + parentTransform.position.x,
                                          fontTransformComp->position.y + parentTransform.position.y,
                                          fontTransformComp->scale.x * parentTransform.scale.x,
                                          textLabelComponent->color);
    }
}
