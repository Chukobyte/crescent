#include "ecs_manager.h"

#include "component/component.h"
#include "system/ec_system.h"
#include "system/sprite_rendering_ec_system.h"
#include "../utils/rbe_assert.h"
#include "system/script_ec_system.h"
#include "../scripting/python/py_script_context.h"

void rbe_ecs_manager_initialize() {
    component_manager_initialize();
    rbe_ec_system_initialize();
    // Initialize and register ec systems
    rbe_ec_system_register(sprite_rendering_ec_system_create());
    rbe_ec_system_register(script_ec_system_create(rbe_py_create_script_context())); // Defaults to python script context
}

void rbe_ecs_manager_finalize() {
    rbe_ec_system_finalize();
    component_manager_finalize();
}
