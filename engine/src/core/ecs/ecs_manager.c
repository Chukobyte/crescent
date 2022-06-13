#include "ecs_manager.h"

#include <string.h>

#include "component/component.h"
#include "system/ec_system.h"
#include "system/sprite_rendering_ec_system.h"
#include "../utils/rbe_assert.h"
#include "system/script_ec_system.h"
#include "../scene/scene_manager.h"

void rbe_ecs_manager_initialize() {
    component_manager_initialize();
    rbe_ec_system_initialize();
    // Initialize and register ec systems
    rbe_ec_system_register(sprite_rendering_ec_system_create());
    rbe_ec_system_register(script_ec_system_create());
}

void rbe_ecs_manager_temp_internal_test() {
    // Create temp entity
    const Entity fpsDisplayEntity = rbe_ec_system_create_entity();
    ScriptComponent* scriptComponent = script_component_create();
    scriptComponent->contextType = ScriptContextType_NATIVE;
    scriptComponent->classPath = "main";
    scriptComponent->className = "FpsDisplay";
    component_manager_set_component(fpsDisplayEntity, ComponentDataIndex_SCRIPT, scriptComponent);
    rbe_ec_system_update_entity_signature_with_systems(fpsDisplayEntity);
    rbe_scene_manager_queue_entity_for_creation(fpsDisplayEntity);
}

void rbe_ecs_manager_finalize() {
    rbe_ec_system_finalize();
    component_manager_finalize();
}
