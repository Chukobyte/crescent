#include "tilemap_ec_system.h"

#include "../ecs_globals.h"
#include "../../tilemap/tilemap.h"
#include "../components/tilemap_component.h"
#include "seika/ecs/ec_system.h"
#include "seika/utils/se_assert.h"
#include "seika/memory/se_mem.h"

static void on_entity_entered_scene(SkaECSSystem* system, SkaEntity entity);
static void on_entity_unregistered(SkaECSSystem* system, SkaEntity entity);

void cre_tilemap_ec_system_create_and_register() {
    SkaECSSystemTemplate systemTemplate = ska_ecs_system_create_default_template("Parallax");
    systemTemplate.on_entity_registered_func = on_entity_entered_scene;
    systemTemplate.on_entity_unregistered_func = on_entity_unregistered;
    SKA_ECS_SYSTEM_REGISTER_FROM_TEMPLATE(&systemTemplate, Transform2DComponent, TilemapComponent);
}

void on_entity_entered_scene(SkaECSSystem* system, SkaEntity entity) {}

void on_entity_unregistered(SkaECSSystem* system, SkaEntity entity) {
    TilemapComponent* tilemapComponent = (TilemapComponent*)ska_ecs_component_manager_get_component(entity, PARTICLES2D_COMPONENT_INDEX);
    SE_ASSERT(tilemapComponent->tilemap);
    cre_tilemap_finalize(tilemapComponent->tilemap);
    SE_MEM_FREE(tilemapComponent->tilemap);
    tilemapComponent->tilemap = NULL;
}
