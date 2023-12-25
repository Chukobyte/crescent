#include "font_rendering_ec_system.h"

#include <seika/asset/asset_manager.h>
#include <seika/rendering/renderer.h>
#include <seika/utils/se_string_util.h>
#include <seika/utils/se_assert.h>

#include "ec_system.h"
#include "../../engine_context.h"
#include "../../scene/scene_manager.h"
#include "../../camera/camera.h"
#include "../../camera/camera_manager.h"
#include "../component/text_label_component.h"


CreEntitySystem* fontRenderingSystem = NULL;

static void font_rendering_system_render();
static void font_rendering_system_on_ec_system_entity_registered(CreEntity entity);
static void font_rendering_system_on_ec_system_destroy();

CreEntitySystem* cre_font_rendering_ec_system_create() {
    SE_ASSERT(fontRenderingSystem == NULL);
    fontRenderingSystem = cre_ec_system_create();
    fontRenderingSystem->name = se_strdup("Font Rendering");
    fontRenderingSystem->render_func = font_rendering_system_render;
    fontRenderingSystem->on_ec_system_destroy = font_rendering_system_on_ec_system_destroy;
    fontRenderingSystem->on_entity_registered_func = font_rendering_system_on_ec_system_entity_registered;
    fontRenderingSystem->component_signature = CreComponentType_TRANSFORM_2D | CreComponentType_TEXT_LABEL;
    return fontRenderingSystem;
}

static void font_rendering_system_render() {
    const CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    const CRECamera2D* defaultCamera = cre_camera_manager_get_default_camera();
    for (size_t i = 0; i < fontRenderingSystem->entity_count; i++) {
        const CreEntity entity = fontRenderingSystem->entities[i];
        Transform2DComponent* fontTransformComp = (Transform2DComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_TRANSFORM_2D);
        TextLabelComponent* textLabelComponent = (TextLabelComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_TEXT_LABEL);
        const CRECamera2D* renderCamera = fontTransformComp->ignoreCamera ? defaultCamera : camera2D;
        fontTransformComp->isGlobalTransformDirty = true;
        const SKATransformModel2D* globalTransform = cre_scene_manager_get_scene_node_global_transform(entity, fontTransformComp);

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

static void font_rendering_system_on_ec_system_entity_registered(CreEntity entity) {
    // Set default font if none is set already
    TextLabelComponent* textLabelComponent = (TextLabelComponent*)cre_component_manager_get_component_unchecked(entity, CreComponentDataIndex_TEXT_LABEL);
    if (!textLabelComponent->font) {
        textLabelComponent->font = se_asset_manager_get_font(CRE_DEFAULT_FONT_KEY);
    }
}

static void font_rendering_system_on_ec_system_destroy() {
    SE_ASSERT(fontRenderingSystem != NULL);
    fontRenderingSystem = NULL;
}
