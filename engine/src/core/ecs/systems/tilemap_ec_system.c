#include "tilemap_ec_system.h"

#include <seika/ecs/ec_system.h>
#include <seika/memory/se_mem.h>
#include <seika/utils/se_assert.h>

#include "../ecs_globals.h"
#include "../../tilemap/tilemap.h"
#include "../components/tilemap_component.h"
#include "../components/transform2d_component.h"
#include "../../camera/camera.h"
#include "../../camera/camera_manager.h"
#include "../../scene/scene_manager.h"

static void on_entity_unregistered(SkaECSSystem* system, SkaEntity entity);
static void tilemap_render(SkaECSSystem* system);

void cre_tilemap_ec_system_create_and_register() {
    SkaECSSystemTemplate systemTemplate = ska_ecs_system_create_default_template("Tilemap");
    systemTemplate.on_entity_unregistered_func = on_entity_unregistered;
    systemTemplate.render_func = tilemap_render;
    SKA_ECS_SYSTEM_REGISTER_FROM_TEMPLATE(&systemTemplate, Transform2DComponent, TilemapComponent);
}

void on_entity_unregistered(SkaECSSystem* system, SkaEntity entity) {
    TilemapComponent* tilemapComponent = (TilemapComponent*)ska_ecs_component_manager_get_component(entity, TILEMAP_COMPONENT_INDEX);
    SE_ASSERT(tilemapComponent->tilemap);
    cre_tilemap_finalize(tilemapComponent->tilemap);
    SE_MEM_FREE(tilemapComponent->tilemap);
    tilemapComponent->tilemap = NULL;
}

void tilemap_render(SkaECSSystem* system) {
    const CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    const CRECamera2D* defaultCamera = cre_camera_manager_get_default_camera();
    for (size_t i = 0; i < system->entity_count; i++) {
        const SkaEntity entity = system->entities[i];
        Transform2DComponent* tilemapTransformComp = (Transform2DComponent*)ska_ecs_component_manager_get_component(entity, TRANSFORM2D_COMPONENT_INDEX);
        TilemapComponent* tilemapComponent = (TilemapComponent*)ska_ecs_component_manager_get_component(entity, TILEMAP_COMPONENT_INDEX);
        SE_ASSERT(tilemapComponent->tilemap);
        SE_ASSERT(tilemapComponent->tilemap->tilesArray);
        SE_ASSERT(tilemapComponent->tilemap->tileset.texture);
        const CRECamera2D* renderCamera = tilemapTransformComp->ignoreCamera ? defaultCamera : camera2D;
        const SceneNodeRenderResource renderResource = cre_scene_manager_get_scene_node_global_render_resource(entity, tilemapTransformComp, &tilemapComponent->origin);
        const SKATransform2D baseTileTransform = renderResource.transform2D;
        const SKASize2D baseTileSize = {
            .w = (float)tilemapComponent->tilemap->tileset.tileSize.w * renderCamera->zoom.x,
            .h = (float)tilemapComponent->tilemap->tileset.tileSize.h * renderCamera->zoom.y
        };
    }
}
