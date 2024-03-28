#include "color_rect_ec_system.h"

#include <seika/rendering/renderer.h>
#include <seika/ecs/ecs.h>
#include <seika/string.h>
#include <seika/assert.h>

#include "../ecs_globals.h"
#include "../components/transform2d_component.h"
#include "../components/color_rect_component.h"
#include "../../scene/scene_manager.h"
#include "../../camera/camera.h"
#include "../../camera/camera_manager.h"

static SkaTexture* colorRectTexture = NULL;
static SkaRect2 colorRectDrawSource = { 0.0f, 0.0f, 1.0f, 1.0f };

static void on_ec_system_registered(SkaECSSystem* system);
static void on_ec_system_destroyed(SkaECSSystem* system);
static void color_rect_render(SkaECSSystem* system);

void cre_color_rect_ec_system_create_and_register() {
    cre_color_rect_ec_system_create_and_register_ex(NULL);
}

void cre_color_rect_ec_system_create_and_register_ex(struct SkaTexture* rectTexture) {
    colorRectTexture = rectTexture;

    SkaECSSystemTemplate systemTemplate = ska_ecs_system_create_default_template("Collision");
    systemTemplate.on_ec_system_register = on_ec_system_registered;
    systemTemplate.on_ec_system_destroy = on_ec_system_destroyed;
    systemTemplate.render_func = color_rect_render;
    SKA_ECS_SYSTEM_REGISTER_FROM_TEMPLATE(&systemTemplate, Transform2DComponent, ColorRectComponent);
}

void on_ec_system_registered(SkaECSSystem* system) {
    if (colorRectTexture == NULL) {
        colorRectTexture = ska_texture_create_solid_colored_texture(1, 1, 255);
    }
}

void on_ec_system_destroyed(SkaECSSystem* system) {
    if (colorRectTexture) {
        ska_texture_delete(colorRectTexture);
        colorRectTexture = NULL;
    }
}

void color_rect_render(SkaECSSystem* system) {
    const CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    const CRECamera2D* defaultCamera = cre_camera_manager_get_default_camera();

    for (size_t i = 0; i < system->entity_count; i++) {
        const SkaEntity entity = system->entities[i];
        Transform2DComponent* transformComp = (Transform2DComponent*)ska_ecs_component_manager_get_component(entity, TRANSFORM2D_COMPONENT_INDEX);
        const ColorRectComponent* colorRectComponent = (ColorRectComponent*)ska_ecs_component_manager_get_component(entity, COLOR_RECT_COMPONENT_INDEX);
        const CRECamera2D* renderCamera = transformComp->ignoreCamera ? defaultCamera : camera2D;
        const SceneNodeRenderResource renderResource = cre_scene_manager_get_scene_node_global_render_resource(entity, transformComp, &SKA_VECTOR2_ZERO);
        const SkaSize2D destinationSize = {
            .w = colorRectComponent->size.w * renderCamera->zoom.x,
            .h = colorRectComponent->size.h * renderCamera->zoom.y
        };

        ska_renderer_queue_sprite_draw(
            colorRectTexture,
            colorRectDrawSource,
            destinationSize,
            colorRectComponent->color,
            false,
            false,
            &renderResource.transform2D,
            renderResource.globalZIndex,
            NULL
        );
    }
}
