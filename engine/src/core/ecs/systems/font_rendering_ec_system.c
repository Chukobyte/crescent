#include "font_rendering_ec_system.h"

#include <seika/asset/asset_manager.h>
#include <seika/rendering/renderer.h>
#include <seika/ecs/ecs.h>
#include <seika/utils/se_assert.h>

#include "../ecs_globals.h"
#include "../../engine_context.h"
#include "../../scene/scene_manager.h"
#include "../../camera/camera.h"
#include "../../camera/camera_manager.h"
#include "../components/text_label_component.h"

static void font_render(SkaECSSystem* system);
static void on_entity_registered(SkaECSSystem* system, SkaEntity entity);

void cre_font_rendering_ec_system_create_and_register() {
    SkaECSSystemTemplate systemTemplate = ska_ecs_system_create_default_template("Font Rendering");
    systemTemplate.on_entity_registered_func = on_entity_registered;
    systemTemplate.render_func = font_render;
    SKA_ECS_SYSTEM_REGISTER_FROM_TEMPLATE(&systemTemplate, Transform2DComponent, TextLabelComponent);
}

void on_entity_registered(SkaECSSystem* system, SkaEntity entity) {
    // Set default font if none is set already
    TextLabelComponent* textLabelComponent = (TextLabelComponent*)ska_ecs_component_manager_get_component(entity, TEXT_LABEL_COMPONENT_INDEX);
    if (!textLabelComponent->font) {
        textLabelComponent->font = se_asset_manager_get_font(CRE_DEFAULT_FONT_KEY);
    }
}

void font_render(SkaECSSystem* system) {
    const CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    const CRECamera2D* defaultCamera = cre_camera_manager_get_default_camera();
    for (size_t i = 0; i < system->entity_count; i++) {
        const SkaEntity entity = system->entities[i];
        Transform2DComponent* fontTransformComp = (Transform2DComponent*)ska_ecs_component_manager_get_component(entity, TRANSFORM2D_COMPONENT_INDEX);
        TextLabelComponent* textLabelComponent = (TextLabelComponent*)ska_ecs_component_manager_get_component(entity, TEXT_LABEL_COMPONENT_INDEX);
        const CRECamera2D* renderCamera = fontTransformComp->ignoreCamera ? defaultCamera : camera2D;
        const SceneNodeRenderResource renderResource = cre_scene_manager_get_scene_node_global_render_resource(entity, fontTransformComp, &SKA_VECTOR2_ZERO);

        se_renderer_queue_font_draw_call(
            textLabelComponent->font,
            textLabelComponent->text,
            (renderResource.transform2D.position.x - renderCamera->viewport.x + renderCamera->offset.x) * renderCamera->zoom.x,
            (renderResource.transform2D.position.y - renderCamera->viewport.y + renderCamera->offset.y) * renderCamera->zoom.y,
            fontTransformComp->localTransform.scale.x * renderResource.transform2D.scale.x * renderCamera->zoom.x,
            textLabelComponent->color,
            renderResource.globalZIndex
        );
    }
}
