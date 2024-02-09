#include "font_rendering_ec_system.h"

#include <seika/asset/asset_manager.h>
#include <seika/rendering/renderer.h>
#include <seika/ecs/ecs.h>
#include <seika/utils/se_assert.h>

#include "../ecs_globals.h"
//#include "ec_system.h"
#include "../../engine_context.h"
#include "../../scene/scene_manager.h"
#include "../../camera/camera.h"
#include "../../camera/camera_manager.h"
#include "../component/text_label_component.h"

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
        const CreEntity entity = system->entities[i];
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

//CreEntitySystem* fontRenderingSystem = NULL;
//
//static void font_rendering_system_render();
//static void font_rendering_system_on_ec_system_entity_registered(CreEntity entity);
//static void font_rendering_system_on_ec_system_destroy();
//
//CreEntitySystem* cre_font_rendering_ec_system_create() {
//    SE_ASSERT(fontRenderingSystem == NULL);
//    fontRenderingSystem = cre_ec_system_create();
//    fontRenderingSystem->name = se_strdup("Font Rendering");
//    fontRenderingSystem->render_func = font_rendering_system_render;
//    fontRenderingSystem->on_ec_system_destroy = font_rendering_system_on_ec_system_destroy;
//    fontRenderingSystem->on_entity_registered_func = font_rendering_system_on_ec_system_entity_registered;
//    fontRenderingSystem->component_signature = CreComponentType_TRANSFORM_2D | CreComponentType_TEXT_LABEL;
//    return fontRenderingSystem;
//}
//
//static void font_rendering_system_render() {
//    const CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
//    const CRECamera2D* defaultCamera = cre_camera_manager_get_default_camera();
//    for (size_t i = 0; i < fontRenderingSystem->entity_count; i++) {
//        const CreEntity entity = fontRenderingSystem->entities[i];
//        Transform2DComponent* fontTransformComp = (Transform2DComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_TRANSFORM_2D);
//        TextLabelComponent* textLabelComponent = (TextLabelComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_TEXT_LABEL);
//        const CRECamera2D* renderCamera = fontTransformComp->ignoreCamera ? defaultCamera : camera2D;
//        const SceneNodeRenderResource renderResource = cre_scene_manager_get_scene_node_global_render_resource(entity, fontTransformComp, &SKA_VECTOR2_ZERO);
//
//        se_renderer_queue_font_draw_call(
//            textLabelComponent->font,
//            textLabelComponent->text,
//            (renderResource.transform2D.position.x - renderCamera->viewport.x + renderCamera->offset.x) * renderCamera->zoom.x,
//            (renderResource.transform2D.position.y - renderCamera->viewport.y + renderCamera->offset.y) * renderCamera->zoom.y,
//            fontTransformComp->localTransform.scale.x * renderResource.transform2D.scale.x * renderCamera->zoom.x,
//            textLabelComponent->color,
//            renderResource.globalZIndex
//        );
//    }
//}
//
//static void font_rendering_system_on_ec_system_entity_registered(CreEntity entity) {
//    // Set default font if none is set already
//    TextLabelComponent* textLabelComponent = (TextLabelComponent*)cre_component_manager_get_component_unchecked(entity, CreComponentDataIndex_TEXT_LABEL);
//    if (!textLabelComponent->font) {
//        textLabelComponent->font = se_asset_manager_get_font(CRE_DEFAULT_FONT_KEY);
//    }
//}
//
//static void font_rendering_system_on_ec_system_destroy() {
//    SE_ASSERT(fontRenderingSystem != NULL);
//    fontRenderingSystem = NULL;
//}
