#include "ecs_manager.h"

#include "component/component.h"
#include "component/transform2d_component.h"
#include "component/sprite_component.h"
#include "system/ec_system.h"
#include "system/sprite_rendering_ec_system.h"
#include "../asset_manager.h"
#include "../utils/rbe_assert.h"

void rbe_ecs_manager_initialize() {
    component_manager_initialize();
    rbe_ec_system_initialize();
    // Initialize and register ec systems
    rbe_ec_system_register(sprite_rendering_system_create());
}

void rbe_ecs_manager_finalize() {
    rbe_ec_system_finalize();
    component_manager_finalize();
}
