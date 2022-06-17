#include "font_rendering_ec_system.h"

#include "string.h"

#include "ec_system.h"
#include "../component/transform2d_component.h"
#include "../component/text_label_component.h"
#include "../../rendering/renderer.h"
#include "../../utils/rbe_assert.h"
#include "../component/transform2d_component.h"


EntitySystem* fontRenderingSystem = NULL;

void font_rendering_system_render();

EntitySystem* font_rendering_ec_system_create() {
    RBE_ASSERT(fontRenderingSystem == NULL);
    fontRenderingSystem = rbe_ec_system_create();
    // NOTE(PetrFlajsingr): replaced strdup with _strdup since the former is not part of the standard
    fontRenderingSystem->name = _strdup("Font Rendering");
    fontRenderingSystem->render_func = font_rendering_system_render;
    fontRenderingSystem->component_signature = ComponentType_TRANSFORM_2D | ComponentType_TEXT_LABEL;
    return fontRenderingSystem;
}

void font_rendering_system_render() {
    for (size_t i = 0; i < fontRenderingSystem->entity_count; i++) {
        Entity entity = fontRenderingSystem->entities[i];
        Transform2DComponent* fontTransformComp = (Transform2DComponent*) component_manager_get_component(entity, ComponentDataIndex_TRANSFORM_2D);
        TextLabelComponent* textLabelComponent = (TextLabelComponent*) component_manager_get_component(entity, ComponentDataIndex_TEXT_LABEL);
        rbe_renderer_queue_font_draw_call(
            textLabelComponent->font,
            textLabelComponent->text,
            fontTransformComp->position.x,
            fontTransformComp->position.y,
            fontTransformComp->scale.x,
            textLabelComponent->color
        );
    }
}
