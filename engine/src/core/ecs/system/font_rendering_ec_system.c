#include "font_rendering_ec_system.h"

#include "ec_system.h"
#include "../component/transform2d_component.h"
#include "../component/text_label_component.h"
#include "../../rendering/renderer.h"
#include "../../utils/rbe_string_util.h"
#include "../../utils/rbe_assert.h"


EntitySystem* fontRenderingSystem = NULL;

void font_rendering_system_render();
void font_rendering_system_entity_register(Entity entity);
void font_rendering_system_entity_unregister(Entity entity);

EntitySystem* font_rendering_ec_system_create() {
    RBE_ASSERT(fontRenderingSystem == NULL);
    fontRenderingSystem = rbe_ec_system_create();
    fontRenderingSystem->name = rbe_strdup("Font Rendering");
    fontRenderingSystem->render_func = font_rendering_system_render;
    fontRenderingSystem->on_entity_registered_func = font_rendering_system_entity_register;
    fontRenderingSystem->on_entity_unregistered_func = font_rendering_system_entity_unregister;
    fontRenderingSystem->component_signature = ComponentType_TRANSFORM_2D | ComponentType_TEXT_LABEL;
    return fontRenderingSystem;
}

void font_rendering_system_render() {
    for (size_t i = 0; i < fontRenderingSystem->entity_count; i++) {
        const Entity entity = fontRenderingSystem->entities[i];
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

void font_rendering_system_entity_register(Entity entity) {
    rbe_logger_error("[NOT ERROR] font rendering Entity '%d' registered! Entity count = '%d'", entity, fontRenderingSystem->entity_count + 1);
}

void font_rendering_system_entity_unregister(Entity entity) {
    rbe_logger_error("[NOT ERROR] font rendering Entity '%d' unregistered! Entity count = '%d'", entity, fontRenderingSystem->entity_count - 1);
}
