#include "ecs_manager.h"

#include "component/component.h"
#include "component/transform2d_component.h"
#include "component/sprite_component.h"
#include "system/ec_system.h"
#include "system/sprite_rendering_ec_system.h"
#include "../asset_manager.h"

void rbe_ecs_manager_initialize() {
    component_manager_initialize();
    rbe_ec_system_initialize();
    // Initialize and register ec systems
    rbe_ec_system_register(sprite_rendering_system_create());
    // TODO: Temp section to showcase creating a sprite entity, move this into a test.
    // Sprite
    Entity spriteEntity = rbe_ec_system_create_entity();
    Transform2DComponent* spriteTransformComp = transform2d_component_create();
    spriteTransformComp->position.x = 300.0f;
    spriteTransformComp->position.y = 300.0f;
    spriteTransformComp->scale.x = 2.0f;
    spriteTransformComp->scale.y = 2.0f;
    component_manager_set_component(spriteEntity, ComponentDataIndex_TRANSFORM_2D, spriteTransformComp);
    SpriteComponent* spriteComponent = sprite_component_create();
    spriteComponent->texture = rbe_asset_manager_get_texture("walk");
    spriteComponent->drawSource.w = 32.0f;
    spriteComponent->drawSource.h = 32.0f;
    component_manager_set_component(spriteEntity, ComponentDataIndex_SPRITE, spriteComponent);
    rbe_ec_system_register_entity_to_systems(spriteEntity);
}

void rbe_ecs_manager_finalize() {
    rbe_ec_system_finalize();
    component_manager_finalize();
}
