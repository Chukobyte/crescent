#include "tilemap_ec_system.h"

#include <seika/ecs/ec_system.h>
#include <seika/rendering/renderer.h>
#include <seika/memory.h>
#include <seika/assert.h>

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
    SKA_ASSERT(tilemapComponent->tilemap);
    cre_tilemap_finalize(tilemapComponent->tilemap);
    SKA_FREE(tilemapComponent->tilemap);
    tilemapComponent->tilemap = NULL;
}

void tilemap_render(SkaECSSystem* system) {
    const CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    const CRECamera2D* defaultCamera = cre_camera_manager_get_default_camera();
    SKA_ECS_SYSTEM_ENTITIES_FOR(system, entity) {
        Transform2DComponent* tilemapTransformComp = (Transform2DComponent*)ska_ecs_component_manager_get_component(entity, TRANSFORM2D_COMPONENT_INDEX);
        TilemapComponent* tilemapComponent = (TilemapComponent*)ska_ecs_component_manager_get_component(entity, TILEMAP_COMPONENT_INDEX);
        SKA_ASSERT(tilemapComponent->tilemap);
        SKA_ASSERT(tilemapComponent->tilemap->tilesArray);
        SKA_ASSERT(tilemapComponent->tilemap->tileset.texture);
        const CRECamera2D* renderCamera = tilemapTransformComp->ignoreCamera ? defaultCamera : camera2D;
        const SceneNodeRenderResource renderResource = cre_scene_manager_get_scene_node_global_render_resource(entity, tilemapTransformComp, &tilemapComponent->origin);
        const SkaTransform2D baseTileTransform = renderResource.transform2D;
        const SkaSize2D baseTileSize = {
            .w = (float)tilemapComponent->tilemap->tileset.tileSize.w * renderCamera->zoom.x,
            .h = (float)tilemapComponent->tilemap->tileset.tileSize.h * renderCamera->zoom.y
        };
        for (size_t j = 0; j < tilemapComponent->tilemap->activeTiles->size; j++) {
            const CreTileData* tileData = (CreTileData*)*(CreTileData**)ska_array_list_get(tilemapComponent->tilemap->activeTiles, j);
            SKA_ASSERT(tileData);
            // TODO: Check if this is the correct calculation for tile position
            const SkaVector2 tilePosition = {
                baseTileTransform.position.x + (float)(tileData->position.x * tilemapComponent->tilemap->tileset.tileSize.w) * baseTileTransform.scale.x,
                baseTileTransform.position.y + (float)(tileData->position.y * tilemapComponent->tilemap->tileset.tileSize.h) * baseTileTransform.scale.y
            };
            const SkaTransform2D tileTransform = {
                .position = tilePosition,
                .scale = baseTileTransform.scale,
                .rotation = baseTileTransform.rotation
            };
            const SkaRect2 tileDrawSource = {
                .x = (float)(tileData->renderCoords.x * tilemapComponent->tilemap->tileset.tileSize.w),
                .y = (float)(tileData->renderCoords.y * tilemapComponent->tilemap->tileset.tileSize.h),
                .w = (float)tilemapComponent->tilemap->tileset.tileSize.w,
                .h = (float)tilemapComponent->tilemap->tileset.tileSize.h
            };

            ska_renderer_queue_sprite_draw(
                tilemapComponent->tilemap->tileset.texture,
                tileDrawSource,
                baseTileSize,
                SKA_COLOR_WHITE,
                false,
                false,
                &tileTransform,
                tilemapTransformComp->zIndex,
                NULL
            );
        }
    }
}
